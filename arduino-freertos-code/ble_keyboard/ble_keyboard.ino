
#include <BleKeyboard.h>
BleKeyboard bleKeyboard("ESP32-numpad", "cystudio", 100);
bool ble_connected = false;
bool ble_sleep = false;
bool prep_deepsleep = false;
unsigned int ble_connect_retries = 0;
unsigned long last_keypress_ms = 0;

// RTOS structs
TaskHandle_t Handle_TaskLedBlink;
TaskHandle_t Handle_TaskWS2812;
TaskHandle_t Handle_TaskSSD1306;
TaskHandle_t Handle_TaskINA219;
TaskHandle_t Handle_TaskTCA8418;

SemaphoreHandle_t serialMutex;
SemaphoreHandle_t wireMutex;
SemaphoreHandle_t bleMutex;

// Shared variables

// SemaphoreHandle_t pwrMutex;
#include <movingAvg.h>
movingAvg avgV(12),avgI(12),avgPWR(12);
float busvoltage = 0;
float current_mA = 0;
float power_mW = 0;

//SemaphoreHandle_t ledMutex;
bool ws_led_off = false;
uint8_t ws_color = 0;
uint32_t ws_mode = 0;

//SemaphoreHandle_t keyMutex;
const byte ROWS = 4;
const byte COLS = 4;
const char keys_norm[ROWS][COLS] = {
  {KEY_NUM_7,KEY_NUM_8,KEY_NUM_9,KEY_BACKSPACE},
  {KEY_NUM_4,KEY_NUM_5,KEY_NUM_6,KEY_DELETE},
  {KEY_NUM_1,KEY_NUM_2,KEY_NUM_3,KEY_NUM_ENTER},
  {KEY_NUM_0,KEY_NUM_PERIOD,KEY_F24,KEY_F24}, 
};
const char keys_numlock[ROWS][COLS] = {
  {KEY_HOME,KEY_UP_ARROW,KEY_PAGE_UP,KEY_F24},
  {KEY_LEFT_ARROW,KEY_F24,KEY_RIGHT_ARROW,KEY_F24},
  {KEY_END,KEY_DOWN_ARROW,KEY_PAGE_DOWN,KEY_RETURN},
  {KEY_INSERT,KEY_DELETE,KEY_F24,KEY_F24},
};
const char keys_funct[ROWS][COLS] = {
  {KEY_HOME,KEY_UP_ARROW,KEY_END,KEY_PAGE_UP},
  {KEY_LEFT_ARROW,KEY_DOWN_ARROW,KEY_RIGHT_ARROW,KEY_PAGE_DOWN},
  {KEY_F24,KEY_F24,KEY_F24,KEY_RIGHT_SHIFT},
  {KEY_F24,KEY_F24,KEY_F24,KEY_F24},
//  {'C-c','C-v',KEY_ESC,KEY_LEFT_SHIFT},
//  {'C-z','C-up',KEY_F24,KEY_F24}, 
};
const char (*kpd_layout)[ROWS][COLS] = &keys_norm;
String kpd_mode;
bool mac_shortcut = true;


/*
 * LED update section:
 * Blue LED (IO17), WS2812 LED (IO18)
 */

// Assign alias to GPIO pins
#define LED_B 17
#define CH_LED_B 1

static void TaskBlink(void *pvParameters) {

  // ESP32 specific methods for LED pwm
  ledcAttachPin(LED_B, CH_LED_B); ledcSetup(CH_LED_B, 12000, 8);

  //                                                                                                                                                                                                                                                                                                              byte intensity=0;
  for(;;) {
    if(prep_deepsleep){ledcWrite(CH_LED_B, 20);break;}
    //if(ble_sleep){
    //  // 4-second breathing cycle
    //  for(intensity=0;intensity<150;intensity+=1){
    //    ledcWrite(CH_LED_B, intensity);
    //    vTaskDelay(10/portTICK_PERIOD_MS);
    //  }
    //  for(;intensity>0;intensity-=1){
    //    ledcWrite(CH_LED_B, intensity);
    //    vTaskDelay(10/portTICK_PERIOD_MS);
    //  }
    //  ledcWrite(CH_LED_B, 0);
    //  vTaskDelay(600/portTICK_PERIOD_MS);
    //}
    if(!ble_connected){
      // 2-second sequence
      ledcWrite(CH_LED_B, 75);
      vTaskDelay(1000/portTICK_PERIOD_MS);
      ledcWrite(CH_LED_B, 20);
      vTaskDelay(1000/portTICK_PERIOD_MS);
    }else{ // ble_connected==true
      // 2-second beep-beep sequence
      ledcWrite(CH_LED_B, 75);
      vTaskDelay(150/portTICK_PERIOD_MS);
      ledcWrite(CH_LED_B, 0);
      vTaskDelay(50/portTICK_PERIOD_MS);
      ledcWrite(CH_LED_B, 75);
      vTaskDelay(150/portTICK_PERIOD_MS);
      ledcWrite(CH_LED_B, 0);
      vTaskDelay(1150/portTICK_PERIOD_MS);
    }
  }
  vTaskDelete(NULL);
}

#include <WS2812FX.h>
#define WS_LED_COUNT 1
#define WS_LED_PIN 18
#define WS_COLOR_CHOICE 10

uint32_t ws_clist[WS_COLOR_CHOICE] = {
  RED, GREEN, BLUE, WHITE, YELLOW,
  CYAN, MAGENTA, PURPLE, ORANGE, PINK
};

WS2812FX ws2812fx = WS2812FX(
  WS_LED_COUNT, WS_LED_PIN,
  NEO_GRB + NEO_KHZ800
);

static void TaskWs2812Update(void *pvParameters) {

  // Library init
  ws_color = 6; // MAGENTA
  ws_mode = FX_MODE_BREATH;
  ws2812fx.init();
  ws2812fx.setBrightness(75);
  ws2812fx.setSpeed(20);
  ws2812fx.setColor(ws_clist[ws_color]);
  ws2812fx.setMode(ws_mode);
  ws2812fx.start();

  for(;;) {
    if(prep_deepsleep || ws_led_off){
      ws2812fx.setColor(BLACK);
      vTaskDelay(90/portTICK_PERIOD_MS);
    }else{
      ws2812fx.setColor(ws_clist[ws_color]);
    }
    ws2812fx.service();
    vTaskDelay(10/portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
}


/*
 * I2C update section: INA219, SSD1306, TCA4818
 * "SemaphoreHandle_t wireMutex" use aquire mutex before lib calls
 */
#include <Wire.h>

#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

#define I2C_ADDRESS 0x3C
SSD1306AsciiWire oled;
uint8_t oledfields[10];

#define SSD1306_DISPLAYOFF 0xAE
void oled_powersave(void){
    Wire.beginTransmission(I2C_ADDRESS);
    Wire.write((uint8_t)0x00);
    Wire.write((uint8_t)SSD1306_DISPLAYOFF);
    Wire.endTransmission();
}

#define SSD1306_SETCONTRAST 0x81
void oled_dim(void){
    Wire.beginTransmission(I2C_ADDRESS);
    Wire.write((uint8_t)0x00);
    Wire.write((uint8_t)SSD1306_SETCONTRAST);
    Wire.endTransmission();
}

static void TaskOledUpdate(void *pvParameters) {
  
  // Initialize ssd1306 via i2c
  
  xSemaphoreTake(wireMutex, portMAX_DELAY);
  oled.begin(&Adafruit128x32, I2C_ADDRESS);
  oled.setFont(Adafruit5x7); oled.set1X();
  oled.displayRemap(true);oled_dim();
  oled.clear();
  oledfields[0] = oled.fieldWidth(strlen("Bluetooth: "));
  oled.println("Bluetooth: no CONN");
  oledfields[1] = oled.fieldWidth(strlen("PWR: "));
  oled.println("PWR: 0.00V 000mA");
  
  oled.println(""); oled.println("mode: NUMPAD");
  oledfields[2] = oled.fieldWidth(strlen("mode: "));
  xSemaphoreGive(wireMutex);
  vTaskDelay(10/portTICK_PERIOD_MS);
  
  for(;;) {
    xSemaphoreTake(wireMutex, portMAX_DELAY);
    
    oled.setCursor(oledfields[0], 0);
    if(ble_connected){
      oled.print("OKAY");oled.clearToEOL();
    }else{
      oled.print("no CONN");oled.clearToEOL();
    }

    oled.setCursor(oledfields[1], 1);
    if(avgV.getCount()>0 && avgI.getCount()>0){
      oled.print(avgV.getAvg()/(float)1000, 2);oled.print("V ");
      oled.print(avgI.getAvg(), 0);oled.print("mA");
      oled.clearToEOL();
    }
    
    oled.clearField(oledfields[2], 3, 1);
    if(kpd_layout==&keys_norm){kpd_mode="NUMPAD";}
    else if(kpd_layout==&keys_numlock){kpd_mode="NUMLOCK";}
    else if(kpd_layout==&keys_funct){kpd_mode="FUNCT";}
    if(ble_sleep){kpd_mode="SLEEP";}
    oled.print(kpd_mode);
    if(mac_shortcut){oled.print("-MAC");}
    oled.clearToEOL();

    xSemaphoreGive(wireMutex);

    vTaskDelay(200/portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
}

#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;

static void TaskIna219Update(void *pvParameters) {

  // Initialize ina219 via i2c
  xSemaphoreTake(wireMutex, portMAX_DELAY);
  ina219.begin();
  ina219.setCalibration_16V_400mA();
  xSemaphoreGive(wireMutex);
  avgV.begin();avgI.begin();avgPWR.begin();
  vTaskDelay(10/portTICK_PERIOD_MS);

  for(;;) {
    // Get reading from INA219
    xSemaphoreTake(wireMutex, portMAX_DELAY);
    busvoltage = ina219.getBusVoltage_V();  avgV.reading(busvoltage*1000);
    current_mA = ina219.getCurrent_mA();    avgI.reading(current_mA);
    power_mW = ina219.getPower_mW();        avgPWR.reading(power_mW);
    xSemaphoreGive(wireMutex);

    vTaskDelay(20/portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
}

/*
 * Keypad conversion section:
 * 4*4 matrix + 4 push btn
 * 
 * 15 actual keys, C-Enter to change mode
 * uses KEY_F24 to represent no-op
 */

#include <Adafruit_TCA8418.h>

#define TCA8418_INT 32
#define TCA8418_RST 33
Adafruit_TCA8418 keypad;
volatile bool tca8418_event = 0;

void IRAM_ATTR tca8418_irq(){
  tca8418_event=true;
}

void process_gpio(int k, int colrow){
  switch(colrow){
    case 101: // NAV_UP
      if(k&0x80){
        ws_color=(ws_color+1)%WS_COLOR_CHOICE;
      }
      break;
    case 102: // NAV_ENTER
      if(k&0x80){
        ws_led_off=!ws_led_off;
      }
      break;
    case 103: // NAV_DOWN
      if(k&0x80){
        ws_color=(ws_color-1+WS_COLOR_CHOICE)%WS_COLOR_CHOICE;
      }
      break;
    case 108: // PUSH_SW
      if(k&0x80){
        mac_shortcut=!mac_shortcut;
      }
      break;
  }
}

static void TaskTca8418Update(void *pvParameters) {

  // Setup isr/irq for TCA8418
  pinMode(TCA8418_INT, INPUT);
  attachInterrupt(digitalPinToInterrupt(TCA8418_INT), tca8418_irq, FALLING);

  // Initialize tca8418 via i2c
  xSemaphoreTake(wireMutex, portMAX_DELAY);
  keypad.begin(TCA8418_DEFAULT_ADDR, &Wire);
  keypad.matrix(4, 4); keypad.flush();
  keypad.enableInterrupts();
  xSemaphoreGive(wireMutex);
  vTaskDelay(10/portTICK_PERIOD_MS);

  bool fn_btnp = false; // button pressed?

  for(;;) {
    if(tca8418_event){
      xSemaphoreTake(wireMutex, portMAX_DELAY);
      int intStat = keypad.readRegister(TCA8418_REG_INT_STAT);
      if (intStat & 0x02){ // mandated
        keypad.readRegister(TCA8418_REG_GPIO_INT_STAT_1);
        keypad.readRegister(TCA8418_REG_GPIO_INT_STAT_2);
        keypad.readRegister(TCA8418_REG_GPIO_INT_STAT_3);
        keypad.writeRegister(TCA8418_REG_INT_STAT, 2);
      }
  
      while(keypad.available() > 0){
        int k = keypad.getEvent();
        int colrow = (k&0x7F)-1;
        last_keypress_ms = millis();
        
        if(colrow>96){ // gpio range
          process_gpio(k, colrow);
          continue;
        }

        if(colrow==32){ // r3c2: fn btn
          fn_btnp = k&0x80;
          if(fn_btnp){kpd_layout=&keys_funct;}
          else{kpd_layout=&keys_norm;}
        }

        if(!ble_connected){continue;}
        if(xSemaphoreTake(bleMutex,(TickType_t)50)==pdTRUE){
          char keycode = (*kpd_layout)[colrow/10][colrow%10];
          if (keycode==KEY_F24){
            // no-op, or check for combos
            if(fn_btnp&&(k&0x80)){
              switch(colrow){
                case 30:
                  bleKeyboard.press(mac_shortcut?KEY_LEFT_GUI:KEY_LEFT_CTRL);
                  bleKeyboard.press('z');
                  break;
                case 31:
                  bleKeyboard.press(KEY_LEFT_GUI);
                  bleKeyboard.press(mac_shortcut?KEY_UP_ARROW:KEY_TAB);
                  break;
                case 20:
                  bleKeyboard.press(mac_shortcut?KEY_LEFT_GUI:KEY_LEFT_CTRL);
                  bleKeyboard.press('c');
                  break;
                case 21:
                  bleKeyboard.press(mac_shortcut?KEY_LEFT_GUI:KEY_LEFT_CTRL);
                  bleKeyboard.press('v');
                  break;
                case 22:
                  bleKeyboard.press(KEY_ESC);
                  break;
                case 23:
                  // bleKeyboard.press(KEY_RIGHT_SHIFT);
                  break;
              }
              vTaskDelay(1/portTICK_PERIOD_MS);
              bleKeyboard.releaseAll();
            }else if((colrow==32)&&!(k&0x80)){
              bleKeyboard.releaseAll();
            }
          }else if(k&0x80){ //PRESS
            bleKeyboard.press(keycode);
          }else{ //RELEASE
            bleKeyboard.release(keycode);
          }
          xSemaphoreGive(bleMutex);
        }
        
      }
      
      keypad.writeRegister(TCA8418_REG_INT_STAT, 1);
      if(!(keypad.readRegister(TCA8418_REG_INT_STAT)&0x03)){
        // Check again no pending events
        tca8418_event = false;
      }
      xSemaphoreGive(wireMutex);
    }

    vTaskDelay(2/portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
}

/*
 * Deepsleep config section:
 * Serial mostly on this core to avoid using mutex
 * CPU set to 80MHz to save on power
 */

RTC_DATA_ATTR int bootCount = 0;

void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason){
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

void peripheral_powersave(){
  // Implementing ESP32 "Modem Sleep"
  // https://www.mischianti.org/2021/03/10/esp32-power-saving-modem-and-light-sleep-2/
  prep_deepsleep = true;
  btStop(); bleKeyboard.end(); ble_connected=false;
  ble_sleep=true; vTaskDelay(500/portTICK_PERIOD_MS);
  ledcWrite(CH_LED_B, 0);ws2812fx.setColor(BLACK);
  xSemaphoreTake(wireMutex, portMAX_DELAY);
  ina219.powerSave(true);
  oled_powersave();
  xSemaphoreGive(wireMutex);
  vTaskDelay(500/portTICK_PERIOD_MS);
}



void genDeviceName(char* deviceName, char* devicePrefix) {
  // https://github.com/T-vK/ESP32-BLE-Keyboard/issues/178
  // populates the deviceName char with the device Prefix plus a "unique" 4 char code
  uint64_t chipid = ESP.getEfuseMac();
  uint16_t shortid = chipid >> 32;
  char sprintfString[20] = "";
  strcat(sprintfString, devicePrefix);
  strcat(sprintfString, "%04X");
  sprintf(deviceName, sprintfString, shortid);
}



void setup() {

  // Initialize serial at 115200baud: same as debug msgs
  Serial.begin(115200);
  Serial.println("--- Bootstrap msgs ---");
  
  // Increment boot number and print it every reboot
  Serial.println("Boot counter: " + String(bootCount++));
  print_wakeup_reason();
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_32,0);

  // Inspect RTOS core usage
  // Mutex declared, not used until loop() & vTask
  serialMutex = xSemaphoreCreateMutex();
  Serial.print("setup() running on core ");
  Serial.println(xPortGetCoreID());
  Serial.println();
  vTaskDelay(50/portTICK_PERIOD_MS);

  // Initialize I2C bus, together with mutex
  wireMutex = xSemaphoreCreateMutex();
  Wire.begin();
  Wire.setClock(400000L);
  vTaskDelay(50/portTICK_PERIOD_MS);

  // Initialize BLE keyboard, together with mutex
  bleMutex = xSemaphoreCreateMutex();
  char deviceName[20];
  genDeviceName(deviceName, "ESP32kp-");
  bleKeyboard.setName(deviceName);
  bleKeyboard.begin();
  vTaskDelay(50/portTICK_PERIOD_MS);

  // Attach RTOS task to actual cores
  xTaskCreatePinnedToCore(
      TaskBlink, /* Function to implement the task */
      "TaskBlink", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &Handle_TaskLedBlink,  /* Task handle. */
      0); /* Core where the task should run */

  xTaskCreatePinnedToCore(
      TaskOledUpdate, /* Function to implement the task */
      "TaskOledUpdate", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      1,  /* Priority of the task */
      &Handle_TaskSSD1306,  /* Task handle. */
      0); /* Core where the task should run */

  xTaskCreatePinnedToCore(
      TaskIna219Update, /* Function to implement the task */
      "TaskIna219Update", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      1,  /* Priority of the task */
      &Handle_TaskINA219,  /* Task handle. */
      0); /* Core where the task should run */

  xTaskCreatePinnedToCore(
      TaskTca8418Update, /* Function to implement the task */
      "TaskTca8418Update", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      1,  /* Priority of the task */
      &Handle_TaskTCA8418,  /* Task handle. */
      0); /* Core where the task should run */

  xTaskCreatePinnedToCore(
      TaskWs2812Update, /* Function to implement the task */
      "TaskWs2812Update", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      1,  /* Priority of the task */
      &Handle_TaskWS2812,  /* Task handle. */
      0); /* Core where the task should run */
}


void loop() {
  // If BLE is not connected, no-op
  if(xSemaphoreTake(bleMutex,(TickType_t)10)==pdTRUE){
    if(!ble_sleep){
      if(!bleKeyboard.isConnected()){
        ble_connected = false;
        vTaskDelay(500/portTICK_PERIOD_MS);
        ble_connect_retries++;
      }else{
        ble_connected = true;
        ble_connect_retries = 0;
      }
      if(ble_connect_retries>600){ // 600*0.5s attempts
        xSemaphoreTake(serialMutex, portMAX_DELAY);
        Serial.println("No BLE connection for 5mins, entering powersave...");
        xSemaphoreGive(serialMutex);
        peripheral_powersave();esp_deep_sleep_start();
      }
    }
    xSemaphoreGive(bleMutex);
  }
  vTaskDelay(2/portTICK_PERIOD_MS);

  // Power-down & wake-up(reset) handling
  if(!ble_sleep && (millis()-last_keypress_ms > 45*60*1000)){
    xSemaphoreTake(serialMutex, portMAX_DELAY);
    Serial.println("No keypress for 45mins, entering powersave...");
    xSemaphoreGive(serialMutex);
    peripheral_powersave();esp_deep_sleep_start();
  }
}
