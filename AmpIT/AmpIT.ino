#include <EEPROM.h>

#include <IRremote.h>
#include <IRremoteInt.h>
#include <boarddefs.h>
#include <ir_Lego_PF_BitStreamEncoder.h>

#include <Wire.h>
#include <BD37534FV.h>
BD37534FV tda;

#define outputA 3
#define outputB 9
#define S 2
#define yello 22
#define orange 23
#define bron 24
#define red 25
#define green 26

#define headphone 40
#define protection 42
#define muteOP 44
#define powerOP 43

int counter = 0;
int oldCounter = 0;
int aState;
int aLastState;
int s = 1;
int po = 1,mu = 1, u = 1, d = 1, b =1;
#define IR_1 0xFF9A65  // left
#define IR_2 0xFFE817  // right
#define IR_3 0xFFD827  //  up
#define IR_4 0xFF609F  // doqn
#define IR_5 0xFFB04F  // LINE IN
#define IR_6 0xFF30CF  // MUTE
#define IR_7 0xFF12ED  // POWER
#define IR_8 0xFF48B7  // menu
#define IR_9 0xFF0AF5  // back 30 deg


#define A 30  //Spk A
#define B 31  // Spk B



#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);
IRrecv irrecv(12);
decode_results ir;
uint8_t arrow[8] = { 0x0, 0x04, 0x06, 0x1f, 0x06, 0x04, 0x00, 0x00 };
unsigned long time;
int menu = -2, df = 0, page = 0, t = 0, Ready = 0;
int prev = 0, current = 1, vol = -79, input = 0, bassUnit = 0, bassQUnit = 0, middUnit = 0, trebbleUnit = 0, freqUnit = 0, oPhaseUnit = 0;
int loudnessUnit = 0, loudgainUnit = 0, bassGain = 0, power = 0, p = 0, q = 0, mute = 0, m = 0;
int middGain = -5, middQUnit = 0, trebbleGain = 0, trebbleQUnit = 0;
int RightFront = 0, LeftFront = 0, RightRear = 0, LeftRear = 0, sub_out = 0, spk = 0;
int inputGain0 = 0, inputGain1 = 0, inputGain2 = 0, inputGain3 = 0, inputGain4 = 0, inputGain5 = 1;
int loud = 0;
int start = 0;
int m1 = 1;
int k = 1;
int h1 = 0;
int p1 = 0,p2 =1;
byte zero[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};
byte one[] = {
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000
};

byte two[] = {
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000
};

byte three[] = {
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100
};

byte four[] = {
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110
};

byte five[] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

void setup() {
  // put your setup code here, to run once:

  Wire.begin();
  lcd.init();
  lcd.backlight();
  irrecv.enableIRIn();

  pinMode(outputA, INPUT);
  pinMode(outputB, INPUT);

  //  Serial.begin (9600);
  aLastState = digitalRead(outputA);
  Serial.begin(9600);
  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(S, INPUT_PULLUP);
  pinMode(yello, INPUT_PULLUP);
  pinMode(orange, INPUT_PULLUP);
  pinMode(bron, INPUT_PULLUP);
  pinMode(red, INPUT_PULLUP);
  pinMode(green, INPUT_PULLUP);

  pinMode(headphone,INPUT_PULLUP);
  pinMode(protection,INPUT_PULLUP);
  pinMode(muteOP,OUTPUT);
  pinMode(powerOP,OUTPUT);

  lcd.createChar(6, arrow);

  lcd.createChar(0, zero);
  lcd.createChar(1, one);
  lcd.createChar(2, two);
  lcd.createChar(3, three);
  lcd.createChar(4, four);
  lcd.createChar(5, five);

  if (EEPROM.read(100) != 0) {
    for (int i = 0; i < 101; i++) { EEPROM.update(i, 0); }  //clear memory on button
  }
  vol = EEPROM.read(0) - 80;
  input = EEPROM.read(1);
  bassUnit = EEPROM.read(2);
  bassQUnit = EEPROM.read(3);
  bassGain = EEPROM.read(4);
  middUnit = EEPROM.read(5);
  // middGain = EEPROM.read(6);
  middGain = -5;
  middQUnit = EEPROM.read(7);
  trebbleUnit = EEPROM.read(8);
  trebbleGain = EEPROM.read(9);
  trebbleQUnit = EEPROM.read(10);
  // inputGain = EEPROM.read(11);
  freqUnit = EEPROM.read(12);
  oPhaseUnit = EEPROM.read(13);
  loudnessUnit = EEPROM.read(14);
  loudgainUnit = EEPROM.read(15);
  mute = EEPROM.read(16);
  RightFront = EEPROM.read(17);
  LeftFront = EEPROM.read(18);
  RightRear = EEPROM.read(19);
  LeftRear = EEPROM.read(20);
  loud = EEPROM.read(21);
  sub_out = EEPROM.read(22);
  inputGain0 = EEPROM.read(23);
  inputGain1 = EEPROM.read(24);
  inputGain2 = EEPROM.read(25);
  inputGain3 = EEPROM.read(26);
  inputGain4 = EEPROM.read(27);
  inputGain5 = EEPROM.read(28);

  attachInterrupt(digitalPinToInterrupt(3), update, CHANGE);
  time = millis();
}
void startupdate() {
  for (int i = 0; i < vol + 79; i++) {
    updateProgressBar(i, 100, 1);
  }
}
void updateProgressBar(unsigned long count, unsigned long totalCount, int lineToPrintOn) {
  double factor = totalCount / 100.0;  //See note above!
  int percent = (count + 1) / factor;
  int number = percent / 5;
  int remainder = percent % 5;
  if (number > 0) {
    lcd.setCursor(number - 1, lineToPrintOn);
    //  lcd.setCursor(number,lineToPrintOn);/

    lcd.write(5);
  }

  lcd.setCursor(number, lineToPrintOn);
  lcd.write(remainder);
}
void update() {
  aState = digitalRead(outputA);
  if (aState != aLastState) {
    if (digitalRead(outputB) != aState) {
      counter++;
    } else {
      counter--;
    }
    Serial.print("Position: ");
    Serial.println(counter);
  }
  aLastState = aState;
  start = 0;
}
void audio() {
  tda.setSetup_1(power, 2, 0);  // default (1,2,0)
  tda.setSetup_2(freqUnit, sub_out, 0, oPhaseUnit);
  // int sub_f ------ OFF 55Hz 85Hz 120Hz 160Hz = int 0...4
  // int sub_out ---- LPF Front Rear Prohibition = int 0...3
  // int level_metr - HOLD REST = int 0..1
  // int faza ------- 0 180 = int 0...1
  tda.setLoudness_f(loudnessUnit);  // 250Hz 400Hz 800Hz Prohibition = int 0...3
  tda.setIn(input);                 // 0...2
  if(input == 0){
    tda.setIn_gain(inputGain0, mute);  // in_gain -- 0...20 = 0...20 dB, mute -- 0 default
  }
  if(input == 1){
    tda.setIn_gain(inputGain1, mute);  // in_gain -- 0...20 = 0...20 dB, mute -- 0 default
  }
  if(input == 2){
    tda.setIn_gain(inputGain2, mute);  // in_gain -- 0...20 = 0...20 dB, mute -- 0 default
  }
  if(input == 3){
    tda.setIn_gain(inputGain3, mute);  // in_gain -- 0...20 = 0...20 dB, mute -- 0 default
  }
  if(input == 4){
    tda.setIn_gain(inputGain4, mute);  // in_gain -- 0...20 = 0...20 dB, mute -- 0 default
  }
  if(input == 5){
    tda.setIn_gain(inputGain5, mute);  // in_gain -- 0...20 = 0...20 dB, mute -- 0 default
  }
  tda.setVol(vol);                  // -79...+15 dB = int -79...15
  tda.setFront_1(RightFront);       // -79...+15 dB = int -79...15
  tda.setFront_2(LeftFront);        // -79...+15 dB = int -79...15
  tda.setRear_1(RightRear);         // -79...+15 dB = int -79...15
  tda.setRear_2(LeftRear);          // -79...+15 dB = int -79...15
  tda.setSub(0);                    // -79...+15 dB = int -79...15
  tda.mix();
  tda.setBass_setup(bassQUnit, bassUnit);          // 0.5 1.0 1.5 2.0 --- int 0...3, 60Hz 80Hz 100Hz 120Hz --- int 0...3
  tda.setMiddle_setup(middQUnit, middUnit);        // 0.75 1.0 1.25 1.5 --- int 0...3, 500Hz 1kHz 1.5kHz 2.5kHz --- int 0...3
  tda.setTreble_setup(trebbleQUnit, trebbleUnit);  // 0.75 1.25  --- int 0...1, 7.5kHz 10kHz 12.5kHz 15kHz --- int 0...3
  tda.setBass_gain(bassGain);                      // --20 ... +20 dB = int -20 ... 20
  tda.setMiddle_gain(middGain);                    // --20 ... +20 dB = int -20 ... 20
  tda.setTreble_gain(trebbleGain);                 // --20 ... +20 dB = int -20 ... 20
  tda.setLoudness_gain(loud);                      // 0 ... 20 dB = int 0 ... 20
}
void loop() {
  // put your main code here, to run repeatedly:
  // s = digitalRead(S);
  if (power == 0 && q == 0) {
    mute = 1;
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("     POWER OFF ");
    delay(2000);
    digitalWrite(powerOP,0);
    lcd.clear();
    // lcd.setBacklight(LOW);
    lcd.noBacklight();
    // lcd.noDisplay();
    menu = -2;

    Ready = 1;
    p = 0;
    q = 1;
  }
  if(p1 == 0){
    p2 = digitalRead(protection);
  }
  if(p2 == 0){
    if(p1 == 0){
      digitalWrite(muteOP,0);
      // power = 0;
      digitalWrite(powerOP,0);
      lcd.clear();
      lcd.setCursor(0,1);
      lcd.print("     Protection ");
    }
    p1 = 1;
  }
  if(digitalRead(headphone)==0){
    lcd.setCursor(5, 3);
    lcd.print("HEADPHONE ");
    h1 = 1;
    m =1;
  }
  if(digitalRead(headphone)==1){
    if(h1 == 1){
      lcd.setCursor(5, 3);
      lcd.print("          ");
      h1 = 0;
      m =0;
    }
  }  
  if (mute == 1 && power == 1) {
    if (m == 0 && menu == -1) {
      // lcd.clear();
      lcd.setCursor(5, 3);
      lcd.print("SOUND MUTE");
      m = 1;
      m1 = 0;
    }
  }
  if (mute == 0) {
    if (m1 == 0) {
      lcd.setCursor(5, 3);
      lcd.print("           ");
      m1 = 1;
    }
  }
  if (power == 1 && p == 0) {
    time = millis();
    digitalWrite(protection,1);
    // lcd.display();
    lcd.clear();
    // lcd.setCursor(0, 1);
    // lcd.print("     POWER ON ");
    lcd.backlight();
    lcd.setCursor(6,0);
    lcd.print("BAWA-2.1");
    lcd.setCursor(9,1);
    lcd.print("By");
    lcd.setCursor(0,2);
    lcd.print(" Jayson Electronics");
    lcd.setCursor(5,3);
    lcd.print("Leicester.");
    digitalWrite(powerOP,1);
    p2 = 1;
    // Serial.println("here");
    delay(4000);

    menu = -1;
    Ready = 0;
    t = 0;
    p = 1;
    mute = 0;
  }

  else if (power == 1 && menu == -1 && Ready == 0 && p2 != 0) {
    // start = 0;
    if (t == 0) {
      lcd.clear();
      startupdate();
      m = 0;
      t = 1;
    }
    // lcd.setCursor(0, 0);
    // lcd.print("Press Menu");
    lcd.setCursor(1, 0);
    lcd.print("VOLUME : ");
    lcd.setCursor(10, 0);
    lcd.print(vol);
    lcd.print(" ");
    lcd.setCursor(1, 2);
    lcd.print("Speaker :");
    lcd.setCursor(11, 2);
    if (spk == 0) {
      lcd.print("OFF      ");
      digitalWrite(A, LOW);
      digitalWrite(B, LOW);
    }
    if (spk == 1) {
      lcd.print("A       ");
      digitalWrite(A, HIGH);
      digitalWrite(B, LOW);
    }
    if (spk == 2) {
      lcd.print("B     ");
      digitalWrite(B, HIGH);
      digitalWrite(A, LOW);
    }
    if (spk == 3) {
      lcd.print("A + B     ");
      digitalWrite(B, HIGH);
      digitalWrite(A, HIGH);
    }

    if (page == 0) {
      lcd.setCursor(0, 0);
      lcd.write(6);
      lcd.setCursor(0, 2);
      lcd.print(" ");
      if (vol == -80) {
        mute = 1;
        m = 0;
      } else if (vol == -79) {
        mute = 0;
      }
      updateProgressBar(vol + 79, 100, 1);
      // startupdate();
    }
    if (page == 1) {
      lcd.setCursor(0, 2);
      lcd.write(6);
      lcd.setCursor(0, 0);
      lcd.print(" ");
    }
  }
  // if (Ready == 1 && current != prev) {
  if (Ready == 1) {
    // df = 1;
    // Menu 0 basic menu
    if (menu == 0) {
      // lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("Source ");
      lcd.setCursor(1, 1);
      lcd.print("Tone");
      lcd.setCursor(1, 2);
      lcd.print("Subwofer");
      lcd.setCursor(1, 3);
      lcd.print("Loudness");
      if (page == 0) {
        lcd.setCursor(0, 0);
        lcd.write(6);
        lcd.setCursor(0, 1);
        lcd.print(" ");
        lcd.setCursor(0, 2);
        lcd.print(" ");
        lcd.setCursor(0, 3);
        lcd.print(" ");
      } else if (page == 1) {
        lcd.setCursor(0, 0);
        lcd.print(" ");
        lcd.setCursor(0, 1);
        lcd.write(6);
        lcd.setCursor(0, 2);
        lcd.print(" ");
        lcd.setCursor(0, 3);
        lcd.print(" ");
      } else if (page == 2) {
        lcd.setCursor(0, 0);
        lcd.print(" ");
        lcd.setCursor(0, 1);
        lcd.print(" ");
        lcd.setCursor(0, 2);
        lcd.write(6);
        lcd.setCursor(0, 3);
        lcd.print(" ");
      } else if (page == 3) {
        lcd.setCursor(0, 0);
        lcd.print(" ");
        lcd.setCursor(0, 1);
        lcd.print(" ");
        lcd.setCursor(0, 2);
        lcd.print(" ");
        lcd.setCursor(0, 3);
        lcd.write(6);
      }
    }
    //Menu 1 line In sub Menu
    if (menu == 1) {
      // lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("PHONO");
      lcd.setCursor(1, 1);
      lcd.print("TUNER");
      lcd.setCursor(1, 2);
      lcd.print("CD   ");
      lcd.setCursor(1, 3);
      lcd.print("AUX_1");
      if (page == 0) {
        lcd.setCursor(0, 0);
        lcd.write(6);
        lcd.setCursor(0, 1);
        lcd.print(" ");
        lcd.setCursor(0, 2);
        lcd.print(" ");
        lcd.setCursor(0, 3);
        lcd.print(" ");
        lcd.setCursor(10, 0);
        lcd.print(inputGain0);
        lcd.print(" db");
        lcd.setCursor(10, 1);
        lcd.print("     ");
        lcd.setCursor(10, 2);
        lcd.print("     ");
        lcd.setCursor(10, 3);
        lcd.print("     ");
      } 
      else if (page == 1) {
        lcd.setCursor(0, 0);
        lcd.print(" ");
        lcd.setCursor(0, 1);
        lcd.write(6);
        lcd.setCursor(0, 2);
        lcd.print(" ");
        lcd.setCursor(0, 3);
        lcd.print(" ");
        lcd.setCursor(10, 1);
        lcd.print(inputGain1);
        lcd.print(" db");
        lcd.setCursor(10, 0);
        lcd.print("     ");
        lcd.setCursor(10, 2);
        lcd.print("     ");
        lcd.setCursor(10, 3);
        lcd.print("     ");
      } 
      else if (page == 2) {
        lcd.setCursor(0, 0);
        lcd.print(" ");
        lcd.setCursor(0, 1);
        lcd.print(" ");
        lcd.setCursor(0, 2);
        lcd.write(6);
        lcd.setCursor(0, 3);
        lcd.print(" ");
        lcd.setCursor(10, 2);
        lcd.print(inputGain2);
        lcd.print(" db");
        lcd.setCursor(10, 1);
        lcd.print("     ");
        lcd.setCursor(10, 0);
        lcd.print("     ");
        lcd.setCursor(10, 3);
        lcd.print("     ");
      } 
      else if (page == 3) {
        lcd.setCursor(0, 0);
        lcd.print(" ");
        lcd.setCursor(0, 1);
        lcd.print(" ");
        lcd.setCursor(0, 2);
        lcd.print(" ");
        lcd.setCursor(0, 3);
        lcd.write(6);
        lcd.setCursor(10, 3);
        lcd.print(inputGain3);
        lcd.print(" db");
        lcd.setCursor(10, 1);
        lcd.print("     ");
        lcd.setCursor(10, 2);
        lcd.print("     ");
        lcd.setCursor(10, 0);
        lcd.print("     ");
      } 
      else if (page == 4) {
        // lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("TUNER");
        lcd.setCursor(1, 1);
        lcd.print("CD   ");
        lcd.setCursor(1, 2);
        lcd.print("AUX_1");
        lcd.setCursor(0, 3);
        lcd.write(6);
        lcd.print("AUX_2");
        lcd.setCursor(10, 3);
        lcd.print(inputGain4);
        lcd.print(" db");
        lcd.setCursor(10, 1);
        lcd.print("     ");
        lcd.setCursor(10, 2);
        lcd.print("     ");
        lcd.setCursor(10, 0);
        lcd.print("     ");
      } 
      else if (page == 5) {
        // lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("CD   ");
        lcd.setCursor(1, 1);
        lcd.print("AUX_1");
        lcd.setCursor(1, 2);
        lcd.print("AUX_2");
        lcd.setCursor(0, 3);
        lcd.write(6);
        lcd.print("TAPE");
        lcd.setCursor(10, 3);
        lcd.print(inputGain5);
        lcd.print(" db");
        lcd.setCursor(10, 1);
        lcd.print("     ");
        lcd.setCursor(10, 2);
        lcd.print("     ");
        lcd.setCursor(10, 0);
        lcd.print("     ");
      }
    }
    // Menu2 tone sub menu
    if (menu == 2) {
      lcd.setCursor(1, 0);
      lcd.print("Bass");
      lcd.setCursor(1, 1);
      lcd.print("Midd");
      lcd.setCursor(1, 2);
      lcd.print("Trebble");
      lcd.setCursor(10, 0);
      lcd.print(bassGain);
      lcd.print(" ");
      lcd.setCursor(14, 0);
      lcd.print(" db  ");
      lcd.setCursor(10, 1);
      lcd.print(middGain);
      lcd.print(" ");
      lcd.setCursor(14, 1);
      lcd.print(" db  ");
      lcd.setCursor(10, 2);
      lcd.print(trebbleGain);
      lcd.print(" ");
      lcd.setCursor(14, 2);
      lcd.print(" db  ");
      if (page == 0) {
        lcd.setCursor(0, 0);
        lcd.write(6);
        lcd.setCursor(0, 1);
        lcd.print(" ");
        lcd.setCursor(0, 2);
        lcd.print(" ");
        lcd.setCursor(0, 3);
        lcd.print(" ");

        // lcd.setCursor(10, 1);
        // lcd.print("       ");
        // lcd.setCursor(10, 2);
        // lcd.print("       ");
        // lcd.setCursor(10, 3);
        // lcd.print("       ");

      } else if (page == 1) {
        lcd.setCursor(0, 0);
        lcd.print(" ");
        lcd.setCursor(0, 1);
        lcd.write(6);
        lcd.setCursor(0, 2);
        lcd.print(" ");
        lcd.setCursor(0, 3);
        lcd.print(" ");


        // lcd.setCursor(10, 0);
        // lcd.print("       ");
        // lcd.setCursor(10, 2);
        // lcd.print("       ");
        // lcd.setCursor(10, 3);
        // lcd.print("       ");
      } else if (page == 2) {
        lcd.setCursor(0, 0);
        lcd.print(" ");
        lcd.setCursor(0, 1);
        lcd.print(" ");
        lcd.setCursor(0, 2);
        lcd.write(6);
        lcd.setCursor(0, 3);
        lcd.print(" ");


        // lcd.setCursor(10, 0);
        // lcd.print("       ");
        // lcd.setCursor(10, 1);
        // lcd.print("       ");
        // lcd.setCursor(10, 3);
        // lcd.print("       ");
      }
    }
    //Menu 3 Subofer option
    if (menu == 3) {
      lcd.setCursor(1, 0);
      lcd.print("Frequency");
      lcd.setCursor(1, 1);
      lcd.print("LPF Phase ");
      lcd.setCursor(1, 2);
      lcd.print("Output ");
      lcd.setCursor(11, 0);
      if (freqUnit == 0) {
        lcd.print("0 Hz   ");
      }
      if (freqUnit == 1) {
        lcd.print("55 Hz   ");
      }
      if (freqUnit == 2) {
        lcd.print("85 Hz   ");
      }
      if (freqUnit == 3) {
        lcd.print("120 Hz   ");
      }
      if (freqUnit == 4) {
        lcd.print("160 Hz   ");
      }
      lcd.setCursor(12, 1);
      if (oPhaseUnit == 0) {
        lcd.print("0    ");
      }
      if (oPhaseUnit == 1) {
        lcd.print("180   ");
      }
      lcd.setCursor(11, 2);
      if (sub_out == 0) {
        lcd.print("LPF    ");
      }
      if (sub_out == 1) {
        lcd.print("FRONT  ");
      }
      if (sub_out == 2) {
        lcd.print("REAR   ");
      }

      if (page == 0) {
        lcd.setCursor(0, 0);
        lcd.write(6);
        lcd.setCursor(0, 1);
        lcd.print(" ");
        lcd.setCursor(0, 2);
        lcd.print(" ");
        lcd.setCursor(0, 3);
        lcd.print(" ");
        // lcd.setCursor(12, 1);
        // lcd.print("     ");
        // lcd.setCursor(11, 2);
        // lcd.print("        ");

      } else if (page == 1) {
        lcd.setCursor(0, 0);
        lcd.print(" ");
        lcd.setCursor(0, 1);
        lcd.write(6);
        lcd.setCursor(0, 2);
        lcd.print(" ");
        lcd.setCursor(0, 3);
        lcd.print(" ");
        // lcd.setCursor(11, 0);
        // lcd.print("        ");
        // lcd.setCursor(11, 2);
        // lcd.print("        ");

      } else if (page == 2) {
        lcd.setCursor(0, 0);
        lcd.print(" ");
        lcd.setCursor(0, 1);
        lcd.print(" ");
        lcd.setCursor(0, 2);
        lcd.write(6);
        lcd.setCursor(0, 3);
        lcd.print(" ");
        // lcd.setCursor(11, 0);
        // lcd.print("        ");
        // lcd.setCursor(12, 1);
        // lcd.print("        ");
      }
    }
    // menu 4 sound fill
    if (menu == 4) {
      lcd.setCursor(1, 0);
      lcd.print("Loudness Frq");
      lcd.setCursor(1, 1);
      lcd.print("Loudness");
      lcd.setCursor(1, 2);
      lcd.print("Speaker Output");
      lcd.setCursor(14, 0);
      if (loudnessUnit == 0) {
        lcd.print("250 Hz");
      }
      if (loudnessUnit == 1) {
        lcd.print("400 Hz");
      }
      if (loudnessUnit == 2) {
        lcd.print("800 Hz");
      }
      if (loudnessUnit == 3) {
        lcd.print("FLAT   ");
      }
      lcd.setCursor(14, 1);
      lcd.print(loudgainUnit);
      lcd.print("  ");

      if (page == 0) {
        lcd.setCursor(0, 0);
        lcd.write(6);
        lcd.setCursor(0, 1);
        lcd.print(" ");
        lcd.setCursor(0, 2);
        lcd.print(" ");
        lcd.setCursor(0, 3);
        lcd.print(" ");
        // lcd.setCursor(14,1);
        // lcd.print("       ");

      } else if (page == 1) {
        lcd.setCursor(0, 0);
        lcd.print(" ");
        lcd.setCursor(0, 1);
        lcd.write(6);
        lcd.setCursor(0, 2);
        lcd.print(" ");
        lcd.setCursor(0, 3);
        lcd.print(" ");
        // lcd.setCursor(10,0);
        // lcd.print("       ");

      } else if (page == 2) {
        lcd.setCursor(0, 0);
        lcd.print(" ");
        lcd.setCursor(0, 1);
        lcd.print(" ");
        lcd.setCursor(0, 2);
        lcd.write(6);
        lcd.setCursor(0, 3);
        lcd.print(" ");
        // lcd.setCursor(10, 1);
        // lcd.print("       ");
      }
    }
    //menu 5 bass sub menu
    if (menu == 5) {
      lcd.setCursor(1, 0);
      lcd.print("Bass Center Frq.");

      lcd.setCursor(1, 2);
      lcd.print("Bass Q Factor");
      lcd.setCursor(7, 1);
      if (bassUnit == 0) {
        lcd.print("60 Hz  ");
      }
      if (bassUnit == 1) {
        lcd.print("80 Hz  ");
      }
      if (bassUnit == 2) {
        lcd.print("100 Hz   ");
      }
      if (bassUnit == 3) {
        lcd.print("120 Hz   ");
      }
      lcd.setCursor(7, 3);
      if (bassQUnit == 0) {
        lcd.print("0.5 Hz   ");
      }
      if (bassQUnit == 1) {
        lcd.print("1.0 Hz   ");
      }
      if (bassQUnit == 2) {
        lcd.print("1.5 Hz   ");
      }
      if (bassQUnit == 3) {
        lcd.print("2.0 Hz   ");
      }

      if (page == 0) {
        lcd.setCursor(0, 0);
        lcd.write(6);

        lcd.setCursor(0, 1);
        lcd.print(" ");
        lcd.setCursor(0, 2);
        lcd.print(" ");
        lcd.setCursor(0, 3);
        lcd.print(" ");
        // lcd.setCursor(7, 3);
        // lcd.print("        ");

      } else if (page == 1) {
        lcd.setCursor(0, 0);
        lcd.print(" ");
        lcd.setCursor(0, 1);
        lcd.print(" ");
        // lcd.setCursor(7, 1);
        // lcd.print("      ");
        lcd.setCursor(0, 2);
        lcd.write(6);
      }
    }
    // menu 6 sub menu midd
    if (menu == 6) {
      lcd.setCursor(1, 0);
      lcd.print("Midd Center Frq.");
      lcd.setCursor(1, 2);
      lcd.print("Midd Q Factor");
      // lcd.setCursor(7, 3);
      // lcd.print("      ");
      if (middUnit == 0) {
        lcd.setCursor(7, 1);
        lcd.print("500 Hz  ");
      }
      if (middUnit == 1) {
        lcd.setCursor(7, 1);
        lcd.print("1 KHz  ");
      }
      if (middUnit == 2) {
        lcd.setCursor(7, 1);
        lcd.print("1.5 KHz   ");
      }
      if (middUnit == 3) {
        lcd.setCursor(7, 1);
        lcd.print("2.5 KHz   ");
      }

      if (middQUnit == 0) {
        lcd.setCursor(7, 3);
        lcd.print("0.75     ");
      }
      if (middQUnit == 1) {
        lcd.setCursor(7, 3);
        lcd.print("1.0     ");
      }
      if (middQUnit == 2) {
        lcd.setCursor(7, 3);
        lcd.print("1.25     ");
      }
      if (middQUnit == 3) {
        lcd.setCursor(7, 3);
        lcd.print("1.5     ");
      }
      if (page == 0) {
        lcd.setCursor(0, 0);
        lcd.write(6);
        lcd.setCursor(0, 1);
        lcd.print(" ");
        lcd.setCursor(0, 2);
        lcd.print(" ");
        lcd.setCursor(0, 3);
        lcd.print(" ");

      } else if (page == 1) {
        lcd.setCursor(0, 0);
        lcd.print(" ");
        lcd.setCursor(0, 1);
        lcd.print(" ");
        lcd.setCursor(0, 2);
        lcd.write(6);
        lcd.setCursor(0, 3);
        lcd.print(" ");
        // lcd.setCursor(7, 1);
        // lcd.print("      ");
      }
    }
    // menu 7 sub menu trebble
    if (menu == 7) {
      lcd.setCursor(1, 0);
      lcd.print("Trebble Center Frq");
      lcd.setCursor(1, 2);
      lcd.print("Trebble Q Factor");
      if (trebbleUnit == 0) {
        lcd.setCursor(7, 1);
        lcd.print("7.5 KHz  ");
      }
      if (trebbleUnit == 1) {
        lcd.setCursor(7, 1);
        lcd.print("10 KHz  ");
      }
      if (trebbleUnit == 2) {
        lcd.setCursor(7, 1);
        lcd.print("12.5 KHz  ");
      }
      if (trebbleUnit == 3) {
        lcd.setCursor(7, 1);
        lcd.print("15 KHz  ");
      }
      if (trebbleQUnit == 0) {
        lcd.setCursor(7, 3);
        lcd.print("0.75  ");
      }
      if (trebbleQUnit == 1) {
        lcd.setCursor(7, 3);
        lcd.print("1.25  ");
      }

      if (page == 0) {
        lcd.setCursor(0, 0);
        lcd.write(6);
        lcd.setCursor(0, 1);
        lcd.print(" ");
        lcd.setCursor(0, 2);
        lcd.print(" ");
        lcd.setCursor(0, 3);
        lcd.print(" ");
        // lcd.setCursor(7, 3);
        // lcd.print("       ");

      } else if (page == 1) {
        lcd.setCursor(0, 0);
        lcd.print(" ");
        lcd.setCursor(0, 1);
        lcd.print(" ");
        lcd.setCursor(0, 2);
        lcd.write(6);
        lcd.setCursor(0, 3);
        lcd.print(" ");
        // lcd.setCursor(7, 1);
        // lcd.print("       ");

      } else if (page == 2) {
        lcd.setCursor(0, 0);
        lcd.print(" ");
        lcd.setCursor(0, 1);
        lcd.print(" ");
        lcd.setCursor(0, 2);
        lcd.write(6);
        lcd.setCursor(0, 3);
        lcd.print(" ");
      }
    }
    // menu 8 line input submenu

    // menu 14 sub menu freq.

    // menu 15 sub menu speaker output
    if (menu == 15) {
      lcd.setCursor(1, 0);
      lcd.print("Right Front");
      lcd.setCursor(1, 1);
      lcd.print("Left Front");
      lcd.setCursor(1, 2);
      lcd.print("Right Rear");
      lcd.setCursor(1, 3);
      lcd.print("Left Rear");
      lcd.setCursor(13, 0);
      lcd.print(RightFront);
      lcd.print(" db  ");
      lcd.setCursor(13, 1);
      lcd.print(LeftFront);
      lcd.print(" db  ");
      lcd.setCursor(13, 2);
      lcd.print(RightRear);
      lcd.print(" db  ");
      lcd.setCursor(13, 3);
      lcd.print(LeftRear);
      lcd.print(" db  ");

      if (page == 0) {
        lcd.setCursor(0, 0);
        lcd.write(6);
        lcd.setCursor(0, 1);
        lcd.print(" ");
        lcd.setCursor(0, 2);
        lcd.print(" ");
        lcd.setCursor(0, 3);
        lcd.print(" ");
        lcd.setCursor(13, 0);
        lcd.print(RightFront);
        lcd.print(" db  ");
      } else if (page == 1) {
        lcd.setCursor(0, 0);
        lcd.print(" ");
        lcd.setCursor(0, 1);
        lcd.write(6);
        lcd.setCursor(0, 2);
        lcd.print(" ");
        lcd.setCursor(0, 3);
        lcd.print(" ");
        lcd.setCursor(13, 1);
        lcd.print(LeftFront);
        lcd.print(" db  ");
      } else if (page == 2) {
        lcd.setCursor(0, 0);
        lcd.print(" ");
        lcd.setCursor(0, 1);
        lcd.print(" ");
        lcd.setCursor(0, 2);
        lcd.write(6);
        lcd.setCursor(0, 3);
        lcd.print(" ");
        lcd.setCursor(13, 2);
        lcd.print(RightRear);
        lcd.print(" db  ");
      } else if (page == 3) {
        lcd.setCursor(0, 0);
        lcd.print(" ");
        lcd.setCursor(0, 1);
        lcd.print(" ");
        lcd.setCursor(0, 2);
        lcd.print(" ");
        lcd.setCursor(0, 3);
        lcd.write(6);
        lcd.setCursor(13, 3);
        lcd.print(LeftRear);
        lcd.print(" db  ");
      }
    }
  }
  prev = current;

  s = digitalRead(S);
  po = digitalRead(yello);
  mu = digitalRead(orange);
  u = digitalRead(bron);
  d = digitalRead(red);
  b = digitalRead(green);  

  // center btn
  if (s == 0) {
    if (menu == -1) {
      Ready = 1;
      menu = 0;
    } else if (menu == 0) {
      if (page == 0) {
        menu = 1;
      } else if (page == 1) {
        menu = 2;
      } else if (page == 2) {
        menu = 3;
      } else if (page == 3) {
        menu = 4;
      }
    } else if (menu == 2) {
      if (page == 0) {
        menu = 5;
      }
      if (page == 1) {
        menu = 6;
      }
      if (page == 2) {
        menu = 7;
      }
    } else if (menu == 1) {
      if (page == 0) {
        // menu = 8;
        input = 0;
      } else if (page == 1) {
        // menu = 9;
        input = 1;
      } else if (page == 2) {
        // menu = 10;
        input = 2;
      } else if (page == 3) {
        // menu = 11;
        input = 3;
      } else if (page == 4) {
        // menu = 12;
        input = 4;
      } else if (page == 5) {
        // menu = 13;
        input = 5;
      }
      menu = 0;
    } else if (menu == 4) {
      if (page == 2) {
        menu = 15;
      }
    }
    start = 0;
    page = 0;
    lcd.clear();
  }
  //poer btn 
  if(po == 0){
    if (power == 0) {
      power = 1;
      mute = 0;
    } else {
      power = 0;
      mute = 1;
      m = 0;
      q = 0;
    }
    start = 0;
  }
  //mute btn
  if(mu == 0){
    if (mute == 0) {
      mute = 1;
      m = 0;
    } else {
      mute = 0;
      if (menu == -1)
        lcd.setCursor(5, 3);
      lcd.print("           ");
    }
    start = 0;
  }
  //up btn
  if(u == 0){
    if (menu == -1) {
      if (page == 0) {
        page = 1;
      } else {
        page = 0;
      }
    }
    if (menu == 0 || menu == 14 || menu == 15) {
      if (page == 0) {
        page = 3;
      } else {
        page = page - 1;
      }
    } else if (menu == 1) {
      if (page == 0) {
        page = 5;
      } else {
        page = page - 1;
      }
    } else if (menu == 2 || menu == 4 || menu == 3) {
      if (page == 0) {
        page = 2;
      } else {
        page = page - 1;
      }
    } else if (menu == 5 || menu == 6 || menu == 7) {
      if (page == 0) {
        page = 1;
      } else if (page == 1) {
        page = 0;
      }
    }
    start = 0;
  }
  // don btn 
  if(d == 0){
    if (menu == -1) {
      if (page == 0) {
        page = 1;
      } else {
        page = 0;
      }
    }
    if (menu == 0 || menu == 14 || menu == 15) {
      if (page == 3) {
        page = 0;
      } else {
        page = page + 1;
      }
    } else if (menu == 1) {
      if (page == 5) {
        page = 0;
      } else {
        page = page + 1;
      }
    } else if (menu == 2 || menu == 4 || menu == 3) {
      if (page == 2) {
        page = 0;
      } else {
        page = page + 1;
      }
    } else if (menu == 5 || menu == 6 || menu == 7) {
      if (page == 0) {
        page = 1;
      } else if (page == 1) {
        page = 0;
      }
    }
    start = 0;
  }
  //back btn 
  if(b == 0){
    if (menu == 0) {
      m = 0;
      menu = -1;
      Ready = 0;
      t = 0;
    }
    if (menu == 1 || menu == 2 || menu == 3 || menu == 4) {
      menu = 0;
    } else if (menu == 5 || menu == 6 || menu == 7) {
      menu = 2;
    } else if (menu == 8 || menu == 9 || menu == 10 || menu == 11 || menu == 12 || menu == 13) {
      menu = 1;
    } else if (menu == 14) {
      menu = 3;
    } else if (menu == 15) {
      menu = 4;
    }
    page = 0;
    if (menu != -1) {
      lcd.clear();
    }
    start = 0;
  }
  // rotary
  if (counter > oldCounter) {
    if (menu == -1) {
      if (page == 0) {
        if (vol + (counter - oldCounter) <= 15) {
          vol = vol + (counter - oldCounter);
          updateProgressBar(vol + 79,100,1);
        }
      }
      if (page == 1) {
        if (spk < 3) {
          spk = spk + 1;
        }
      }
    }
    // if (menu == 1) {
    //   if (inputGain + (counter - oldCounter) <= 20 ) {
    //     inputGain = inputGain + (counter - oldCounter);
    //   }
    // } 
    if (menu == 1) {
      if( page == 0){        
        if (inputGain0 + (counter - oldCounter) <= 20) {
          inputGain0 = inputGain0 + (counter - oldCounter);
        }
      }
      if( page == 1){        
        if (inputGain1 + (counter - oldCounter) <= 20) {
          inputGain1 = inputGain1 + (counter - oldCounter);
        }
      }
      if( page == 2){        
        if (inputGain2 + (counter - oldCounter) <= 20) {
          inputGain2 = inputGain2 + (counter - oldCounter);
        }
      }
      if( page == 3){        
        if (inputGain3 + (counter - oldCounter) <= 20) {
          inputGain3 = inputGain3 + (counter - oldCounter);
        }
      }
      if( page == 4){        
        if (inputGain4 + (counter - oldCounter) <= 20) {
          inputGain4 = inputGain4 + (counter - oldCounter);
        }
      }
      if( page == 5){        
        if (inputGain5 + (counter - oldCounter) <= 20) {
          inputGain5 = inputGain5 + (counter - oldCounter);
        }
      }  
    }
    else if (menu == 2) {
      if (page == 0) {
        if (bassGain + (counter - oldCounter) <= 20) {
          bassGain = bassGain + (counter - oldCounter);
        }
      } else if (page == 1) {
        if (middGain + (counter - oldCounter) <= 20) {
          middGain = middGain + (counter - oldCounter);
        }
      } else if (page == 2) {
        if (trebbleGain + (counter - oldCounter) <= 20) {
          trebbleGain = trebbleGain + (counter - oldCounter);
        }
      }
    }
    if (menu == 3) {
      if (page == 0) {
        if (freqUnit < 4) {
          freqUnit = freqUnit + 1;
        }
      }
      if (page == 1) {
        if (oPhaseUnit < 1) {
          oPhaseUnit = oPhaseUnit + 1;
        }
      }
      if (page == 2) {
        if (sub_out < 2) {
          sub_out = sub_out + 1;
        }
      }
    }
    if (menu == 4) {
      if (page == 0) {
        if (loudnessUnit < 3) {
          loudnessUnit = loudnessUnit + 1;
        }
      }
    }
    if (page == 1) {
      if (loudgainUnit < 20) {
        loudgainUnit = loudgainUnit + 1;
        loud = 20 - loudgainUnit;
      }
    }
    if (menu == 5) {
      if (page == 0) {
        if (bassUnit < 3) {
          bassUnit = bassUnit + 1;
        }
      }
      if (page == 1) {
        if (bassQUnit < 3) {
          bassQUnit = bassQUnit + 1;
        }
      }
    }
    if (menu == 6) {
      if (page == 0) {
        if (middUnit < 3) {
          middUnit = middUnit + 1;
        }
      }
      if (page == 1) {
        if (middQUnit < 3) {
          middQUnit = middQUnit + 1;
        }
      }
    }
    if (menu == 7) {
      if (page == 0) {
        if (trebbleUnit < 3) {
          trebbleUnit = trebbleUnit + 1;
        }
      }
      if (page == 1) {
        if (trebbleQUnit < 1) {
          trebbleQUnit = trebbleQUnit + 1;
        }
      }
    }
    if (menu == 15) {
      if (page == 0) {
        if (RightFront + (counter - oldCounter) <= 15) {
          RightFront = RightFront + (counter - oldCounter);
        }
      }
      if (page == 1) {
        if (LeftFront + (counter - oldCounter) <= 15) {
          LeftFront = LeftFront + (counter - oldCounter);
        }
      }
      if (page == 2) {
        if (RightRear + (counter - oldCounter) <= 15) {
          RightRear = RightRear + (counter - oldCounter);
        }
      }
      if (page == 3) {
        if (LeftRear + (counter - oldCounter) <= 15) {
          LeftRear = LeftRear + (counter - oldCounter);
        }
      }
    }
  } 
  else if (counter < oldCounter) {
    if (menu == -1) {
      if (page == 0) {
        if (vol + (counter - oldCounter) >= -80) {
          vol = vol + (counter - oldCounter);
        }
      }
      if (page == 1) {
        if (spk > 0) {
          spk = spk - 1;
        }
      }
    }
    if (menu == 2) {
      if (page == 0) {
        if (bassGain + (counter - oldCounter) >= -20) {
          bassGain = bassGain + (counter - oldCounter);
        }
      } else if (page == 1) {
        if (middGain + (counter - oldCounter) >= -20) {
          middGain = middGain + (counter - oldCounter);
        }
      } else if (page == 2) {
        if (trebbleGain + (counter - oldCounter) >= -20) {
          trebbleGain = trebbleGain + (counter - oldCounter);
        }
      }
    }
    // if (menu == 1) {
    //   if (inputGain + (counter - oldCounter) >= 0) {
    //     inputGain = inputGain + (counter - oldCounter);
    //   }
    // }
    if (menu == 1) {
      if( page == 0){        
        if (inputGain0 + (counter - oldCounter) >= 0) {
          inputGain0 = inputGain0 + (counter - oldCounter);
        }
      }
      if( page == 1){        
        if (inputGain1 + (counter - oldCounter) >= 0) {
          inputGain1 = inputGain1 + (counter - oldCounter);
        }
      }
      if( page == 2){        
        if (inputGain2 + (counter - oldCounter) >= 0) {
          inputGain2 = inputGain2 + (counter - oldCounter);
        }
      }
      if( page == 3){        
        if (inputGain3 + (counter - oldCounter) >= 0) {
          inputGain3 = inputGain3 + (counter - oldCounter);
        }
      }
      if( page == 4){        
        if (inputGain4 + (counter - oldCounter) >= 0) {
          inputGain4 = inputGain4 + (counter - oldCounter);
        }
      }
      if( page == 5){        
        if (inputGain5 + (counter - oldCounter)> 0) {
          inputGain5 = inputGain5 + (counter - oldCounter);
        }
      }  
    }
    if (menu == 3) {
      if (page == 0) {
        if (freqUnit > 1) {
          freqUnit = freqUnit - 1;
        }
      }
      if (page == 1) {
        if (oPhaseUnit > 0) {
          oPhaseUnit = oPhaseUnit - 1;
        }
      }
      if (page == 2) {
        if (sub_out > 0) {
          sub_out = sub_out - 1;
        }
      }
    }
    if (menu == 4) {
      if (page == 0) {
        if (loudnessUnit + (counter - oldCounter) >= 0) {
          loudnessUnit = loudnessUnit + (counter - oldCounter);
        }
      }
      if (page == 1) {
        if (loudgainUnit + (counter - oldCounter) >= 0) {
          loudgainUnit = loudgainUnit + (counter - oldCounter);
          loud = 20 - loudgainUnit;
        }
      }
    }
    if (menu == 5) {
      if (page == 0) {
        if (bassUnit > 0) {
          bassUnit = bassUnit - 1;
        }
      }
      if (page == 1) {
        if (bassQUnit > 0) {
          bassQUnit = bassQUnit - 1;
        }
      }
    }
    if (menu == 6) {
      if (page == 0) {
        if (middUnit > 0) {
          middUnit = middUnit - 1;
        }
      }
      if (page == 1) {
        if (middQUnit > 0) {
          middQUnit = middQUnit - 1;
        }
      }
    }
    if (menu == 7) {
      if (page == 0) {
        if (trebbleUnit > 0) {
          trebbleUnit = trebbleUnit - 1;
        }
      }
      if (page == 1) {
        if (trebbleQUnit > 0) {
          trebbleQUnit = trebbleQUnit - 1;
        }
      }
    }
    if (menu == 15) {
      if (page == 0) {
        if (RightFront + (counter - oldCounter) > -80) {
          RightFront = RightFront + (counter - oldCounter);
        }
      }
      if (page == 1) {
        if (LeftFront + (counter - oldCounter) > -80) {
          LeftFront = LeftFront + (counter - oldCounter);
        }
      }
      if (page == 2) {
        if (RightRear + (counter - oldCounter) > -80) {
          RightRear = RightRear + (counter - oldCounter);
        }
      }
      if (page == 3) {
        if (LeftRear + (counter - oldCounter) > -80) {
          LeftRear = LeftRear + (counter - oldCounter);
        }
      }
    }
  }

  //remote receive 
  if (irrecv.decode(&ir)) {
    Serial.print("0x");
    Serial.println(ir.value, HEX);
    irrecv.resume();
    current = current + 1;
    start = 1;
    // Serial.println("HERE1");
  } 
  else {
    start++;
    // Serial.print("START ");
    // Serial.println(start);
    // Serial.println("HERE2");
  }
  //remote mute btn
  if (ir.value == IR_6 && prev != current && power == 1) {
    if (mute == 0) {
      mute = 1;
      m = 0;
    } else {
      mute = 0;
      if (menu == -1)
        lcd.setCursor(5, 3);
      lcd.print("           ");
    }
  }
  //remote  poer btn
  if (ir.value == IR_7 && prev != current) {
    if (power == 0) {
      power = 1;
      
    } 
    else {
      power = 0;
      mute = 1;
      m = 0;
      q = 0;
    }
  }
  // remote left btn
  if (ir.value == IR_2 && prev != current) {
    if (menu == -1) {
      if (page == 0) {
        if (vol > -80) {
          vol = vol - 1;
        }
      }
      if (page == 1) {
        if (spk > 0) {
          spk = spk - 1;
        }
      }
    }
    if (menu == 2) {
      if (page == 0) {
        if (bassGain > -20) {
          bassGain = bassGain - 1;
        }
      } else if (page == 1) {
        if (middGain > -20) {
          middGain = middGain - 1;
        }
      } else if (page == 2) {
        if (trebbleGain > -20) {
          trebbleGain = trebbleGain - 1;
        }
      }
    }
    if (menu == 1) {
      if( page == 0){        
        if (inputGain0 > 0) {
          inputGain0 = inputGain0 - 1;
        }
      }
      if( page == 1){        
        if (inputGain1 > 0) {
          inputGain1 = inputGain1 - 1;
        }
      }
      if( page == 2){        
        if (inputGain2 > 0) {
          inputGain2 = inputGain2 - 1;
        }
      }
      if( page == 3){        
        if (inputGain3 > 0) {
          inputGain3 = inputGain3 - 1;
        }
      }
      if( page == 4){        
        if (inputGain4 > 0) {
          inputGain4 = inputGain4 - 1;
        }
      }
      if( page == 5){        
        if (inputGain5 > 0) {
          inputGain5 = inputGain5 - 1;
        }
      }  
    }
    if (menu == 3) {
      if (page == 0) {
        if (freqUnit > 1) {
          freqUnit = freqUnit - 1;
        }
      }
      if (page == 1) {
        if (oPhaseUnit > 0) {
          oPhaseUnit = oPhaseUnit - 1;
        }
      }
      if (page == 2) {
        if (sub_out > 0) {
          sub_out = sub_out - 1;
        }
      }
    }
    if (menu == 4) {
      if (page == 0) {
        if (loudnessUnit > 0) {
          loudnessUnit = loudnessUnit - 1;
        }
      }
      if (page == 1) {
        if (loudgainUnit > 0) {
          loudgainUnit = loudgainUnit - 1;
          loud = 20 - loudgainUnit;
        }
      }
    }
    if (menu == 5) {
      if (page == 0) {
        if (bassUnit > 0) {
          bassUnit = bassUnit - 1;
        }
      }
      if (page == 1) {
        if (bassQUnit > 0) {
          bassQUnit = bassQUnit - 1;
        }
      }
    }
    if (menu == 6) {
      if (page == 0) {
        if (middUnit > 0) {
          middUnit = middUnit - 1;
        }
      }
      if (page == 1) {
        if (middQUnit > 0) {
          middQUnit = middQUnit - 1;
        }
      }
    }
    if (menu == 7) {
      if (page == 0) {
        if (trebbleUnit > 0) {
          trebbleUnit = trebbleUnit - 1;
        }
      }
      if (page == 1) {
        if (trebbleQUnit > 0) {
          trebbleQUnit = trebbleQUnit - 1;
        }
      }
    }
    if (menu == 15) {
      if (page == 0) {
        if (RightFront > -80) {
          RightFront = RightFront - 1;
        }
      }
      if (page == 1) {
        if (LeftFront > -80) {
          LeftFront = LeftFront - 1;
        }
      }
      if (page == 2) {
        if (RightRear > -80) {
          RightRear = RightRear - 1;
        }
      }
      if (page == 3) {
        if (LeftRear > -80) {
          LeftRear = LeftRear - 1;
        }
      }
    }
  }
  
  // remote right btn
  if (ir.value == IR_1 && prev != current) {
    if (menu == -1) {
      if (page == 0) {
        if (vol < 15) {
          vol = vol + 1;
        }
      }
      if (page == 1) {
        if (spk < 3) {
          spk = spk + 1;
        }
      }
    }
    if (menu == 1) {
      if( page == 0){        
        if (inputGain0 < 20) {
          inputGain0 = inputGain0 + 1;
        }
      }
      if( page == 1){        
        if (inputGain1 < 20) {
          inputGain1 = inputGain1 + 1;
        }
      }
      if( page == 2){        
        if (inputGain2 < 20) {
          inputGain2 = inputGain2 + 1;
        }
      }
      if( page == 3){        
        if (inputGain3 < 20) {
          inputGain3 = inputGain3 + 1;
        }
      }
      if( page == 4){        
        if (inputGain4 < 20) {
          inputGain4 = inputGain4 + 1;
        }
      }
      if( page == 5){        
        if (inputGain5 < 20) {
          inputGain5 = inputGain5 + 1;
        }
      }  
    } 
    else if (menu == 2) {
      if (page == 0) {
        if (bassGain < 20) {
          bassGain = bassGain + 1;
        }
      } else if (page == 1) {
        if (middGain < 20) {
          middGain = middGain + 1;
        }
      } else if (page == 2) {
        if (trebbleGain < 20) {
          trebbleGain = trebbleGain + 1;
        }
      }
    }
    if (menu == 3) {
      if (page == 0) {
        if (freqUnit < 4) {
          freqUnit = freqUnit + 1;
        }
      }
      if (page == 1) {
        if (oPhaseUnit < 1) {
          oPhaseUnit = oPhaseUnit + 1;
        }
      }
      if (page == 2) {
        if (sub_out < 2) {
          sub_out = sub_out + 1;
        }
      }
    }
    if (menu == 4) {
      if (page == 0) {
        if (loudnessUnit < 3) {
          loudnessUnit = loudnessUnit + 1;
        }
      }
    }
    if (page == 1) {
      if (loudgainUnit < 20) {
        loudgainUnit = loudgainUnit + 1;
        loud = 20 - loudgainUnit;
      }
    }
    if (menu == 5) {
      if (page == 0) {
        if (bassUnit < 3) {
          bassUnit = bassUnit + 1;
        }
      }
      if (page == 1) {
        if (bassQUnit < 3) {
          bassQUnit = bassQUnit + 1;
        }
      }
    }
    if (menu == 6) {
      if (page == 0) {
        if (middUnit < 3) {
          middUnit = middUnit + 1;
        }
      }
      if (page == 1) {
        if (middQUnit < 3) {
          middQUnit = middQUnit + 1;
        }
      }
    }
    if (menu == 7) {
      if (page == 0) {
        if (trebbleUnit < 3) {
          trebbleUnit = trebbleUnit + 1;
        }
      }
      if (page == 1) {
        if (trebbleQUnit < 1) {
          trebbleQUnit = trebbleQUnit + 1;
        }
      }
    }
    if (menu == 15) {
      if (page == 0) {
        if (RightFront < 15) {
          RightFront = RightFront + 1;
        }
      }
      if (page == 1) {
        if (LeftFront < 15) {
          LeftFront = LeftFront + 1;
        }
      }
      if (page == 2) {
        if (RightRear < 15) {
          RightRear = RightRear + 1;
        }
      }
      if (page == 3) {
        if (LeftRear < 15) {
          LeftRear = LeftRear + 1;
        }
      }
    }
  }
  // center btn
  
  Serial.print("Menu ");
  Serial.println(menu);

  // remote center btn
  if (ir.value == IR_8 && prev != current) {
    if (menu == -1) {
      Ready = 1;
      menu = 0;
      page = 0;
    } 
    else if (menu == 0) {
      if (page == 0) {
        menu = 1;
        if(input == 0){
          page = 0;
        }
        if(input == 1){
          page = 1;
        }
        if(input == 2){
          page = 2;
        }
        if(input == 3){
          page = 3;
        }
        if(input == 4){
          page = 4;
        }
        if(input == 5){
          page = 5;
        }
        
      } 
      else if (page == 1) {
        menu = 2;
        page = 0;        
      } 
      else if (page == 2) {
        menu = 3;
        page = 0;
      } 
      else if (page == 3) {
        menu = 4;
        page = 0;
      }
    } 
    else if (menu == 2) {
      if (page == 0) {
        menu = 5;
      }
      if (page == 1) {
        menu = 6;
      }
      if (page == 2) {
        menu = 7;
      }
      page = 0;
    } 
    else if (menu == 1) {
      if (page == 0) {
        // menu = 8;
        input = 0;
      } else if (page == 1) {
        // menu = 9;
        input = 1;
      } else if (page == 2) {
        // menu = 10;
        input = 2;
      } else if (page == 3) {
        // menu = 11;
        input = 3;
      } else if (page == 4) {
        // menu = 12;
        input = 4;
      } else if (page == 5) {
        // menu = 13;
        input = 5;
      }
      menu = 0;
      page = 0;
    } 
    else if (menu == 4) {
      if (page == 2) {
        menu = 15;
      }
      page = 0;
    }
    
    lcd.clear();
  }
  // remote don btn
  if (ir.value == IR_4 && prev != current) {
    if (menu == -1) {
      if (page == 0) {
        page = 1;
      } else {
        page = 0;
      }
    }
    if (menu == 0 || menu == 14 || menu == 15) {
      if (page == 3) {
        page = 0;
      } else {
        page = page + 1;
      }
    } else if (menu == 1) {
      if (page == 5) {
        page = 0;
      } else {
        page = page + 1;
      }
    } else if (menu == 2 || menu == 4 || menu == 3) {
      if (page == 2) {
        page = 0;
      } else {
        page = page + 1;
      }
    } else if (menu == 5 || menu == 6 || menu == 7) {
      if (page == 0) {
        page = 1;
      } else if (page == 1) {
        page = 0;
      }
    }
  }
  //remote up btn
  if (ir.value == IR_3 && prev != current) {
    if (menu == -1) {
      if (page == 0) {
        page = 1;
      } else {
        page = 0;
      }
    }
    if (menu == 0 || menu == 14 || menu == 15) {
      if (page == 0) {
        page = 3;
      } else {
        page = page - 1;
      }
    } else if (menu == 1) {
      if (page == 0) {
        page = 5;
      } else {
        page = page - 1;
      }
    } else if (menu == 2 || menu == 4 || menu == 3) {
      if (page == 0) {
        page = 2;
      } else {
        page = page - 1;
      }
    } else if (menu == 5 || menu == 6 || menu == 7) {
      if (page == 0) {
        page = 1;
      } else if (page == 1) {
        page = 0;
      }
    }
  }
  //remote back btn
  if (ir.value == IR_9 && prev != current) {
    if (menu == 0) {
      m = 0;
      menu = -1;
      Ready = 0;
      t = 0;
    }
    if (menu == 1 || menu == 2 || menu == 3 || menu == 4) {
      menu = 0;
    } else if (menu == 5 || menu == 6 || menu == 7) {
      menu = 2;
    } else if (menu == 8 || menu == 9 || menu == 10 || menu == 11 || menu == 12 || menu == 13) {
      menu = 1;
    } else if (menu == 14) {
      menu = 3;
    } else if (menu == 15) {
      menu = 4;
    }
    page = 0;
    if (menu != -1) {
      lcd.clear();
    }
  }
  digitalWrite(muteOP,mute);
  // digitalWrite(powerOP,power);
  audio();
  delay(100);
  if (current == prev && start >= 40 && menu != -1) {
    t = 0;
    menu = -1;
    page = 0;
    Ready = 0;
    start = 0;
  }
  // else if(current == prev && start >= 50 && menu == -1){
  //   start = 0;
  //   page = 0;
  // }

  if (millis() - time > 10000 && power == 1) {
    EEPROM.update(0, vol + 80);
    EEPROM.update(1, input);
    EEPROM.update(2, bassUnit);
    EEPROM.update(3, bassQUnit);
    EEPROM.update(4, bassGain);
    EEPROM.update(5, middUnit);
    EEPROM.update(6, middGain);
    EEPROM.update(7, middQUnit);
    EEPROM.update(8, trebbleUnit);
    EEPROM.update(9, trebbleGain);
    EEPROM.update(10, trebbleQUnit);
    // EEPROM.update(11, inputGain);
    EEPROM.update(12, freqUnit);
    EEPROM.update(13, oPhaseUnit);
    EEPROM.update(14, loudnessUnit);
    EEPROM.update(15, loudgainUnit);
    EEPROM.update(16, mute);
    EEPROM.update(17, RightFront);
    EEPROM.update(18, LeftFront);
    EEPROM.update(19, RightRear);
    EEPROM.update(20, LeftRear);
    EEPROM.update(21, loud);
    EEPROM.update(22, sub_out);
    EEPROM.update(23, inputGain0);
    EEPROM.update(24, inputGain1);
    EEPROM.update(25, inputGain2);
    EEPROM.update(26, inputGain3);
    EEPROM.update(27, inputGain4);
    EEPROM.update(28, inputGain5);

    time = millis();
  }
  // else {
  //   start = start + 1;
  // }

  //   Serial.print("Page : ");
  //   Serial.println(page);
  //   Serial.print("VOLUME : ");
  //   Serial.println(vol);
  // Serial.print("Current : ");
  // Serial.println(current);

  if (current > 100) { current = 0; }
  oldCounter = counter;
}
 