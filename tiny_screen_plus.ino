/*
 * This is a demo of the SI7021 Humidity Sensor
 * This demo is made for the O Watch kit - http://theowatch.com
 * 
 * This demo is based on the SI7021 library example
 * and uses by Marcus Sorensen <marcus@electron14.com>
 * https://github.com/mlsorensen/SI7021/blob/master/SI7021.h
 * licensed under the GNU GPL v2
 * 
 */

#include <TinyScreen.h>
#include <SI7021.h> //include the sensor library

SI7021 sensor; //declare the sensor
TinyScreen display = TinyScreen(TinyScreenPlus);
int background = TS_16b_Black;

// This function gets the battery VCC internally, you can checkout this link 
// if you want to know more about how: 
// http://atmel.force.com/support/articles/en_US/FAQ/ADC-example
float getVCC() {
  SYSCTRL->VREF.reg |= SYSCTRL_VREF_BGOUTEN;
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  ADC->SAMPCTRL.bit.SAMPLEN = 0x1;
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  ADC->INPUTCTRL.bit.MUXPOS = 0x19;         // Internal bandgap input
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  ADC->CTRLA.bit.ENABLE = 0x01;             // Enable ADC
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  ADC->SWTRIG.bit.START = 1;  // Start conversion
  ADC->INTFLAG.bit.RESRDY = 1;  // Clear the Data Ready flag
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  ADC->SWTRIG.bit.START = 1;  // Start the conversion again to throw out first value
  while ( ADC->INTFLAG.bit.RESRDY == 0 );   // Waiting for conversion to complete
  uint32_t valueRead = ADC->RESULT.reg;
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  ADC->CTRLA.bit.ENABLE = 0x00;             // Disable ADC
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  SYSCTRL->VREF.reg &= ~SYSCTRL_VREF_BGOUTEN;
  float vcc = (1.1 * 1023.0) / valueRead;
  return vcc;
}

// Calculate the battery voltage
float getBattVoltage(void) {
  const int VBATTpin = A4;
  float VCC = getVCC();

  // Use resistor division and math to get the voltage
  float resistorDiv = 0.5;
  float ADCres = 1023.0;
  float battVoltageReading = analogRead(VBATTpin);
  battVoltageReading = analogRead(VBATTpin); // Throw out first value
  float battVoltage = VCC * battVoltageReading / ADCres / resistorDiv;

  return battVoltage;
}

void setup(void) {
  Wire.begin();
  sensor.begin();
  display.begin();
  display.setFlip(1);
  display.on();
  display.setFont(liberationSans_8ptFontInfo);
}

void loop() 
{
  //get the temperature using the library function 
  //temperature is an integer in hundredths
  int tempHundredths = sensor.getCelsiusHundredths();
  float temperature = tempHundredths / 100.0;

  //get humidity from the libdary function
  // humidity is an integer representing percent
  int humidityvalue = sensor.getHumidityPercent();

  float battVoltageReading = getBattVoltage();
  
  display.setCursor(5,5);
  display.print("Sensor Demo");
    
  //print humidity valye
  display.setCursor(5,17);
  display.print("Humidity: ");
  display.print(humidityvalue);
  display.print("%  ");

  display.fontColor(TS_8b_Blue, background); // Because blue is cool
  display.setCursor(5, 29);
  display.print("Batt V = ");
  display.print(battVoltageReading);
  
  //print temperature value
  display.setCursor(5,41);
  display.print("Temp: ");
  //display.print((temperature*9/5+32-13)); //printing value converted to Fahrenheit
  display.print((temperature));
  //display.print(" F  ");
  display.print(" C  ");

  delay(2000);
}
