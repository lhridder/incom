unsigned int pins[8] = {2,3,4,5,6,7,8,9};
unsigned int npins = 8;
bool mode = 0;
bool reading = 0;
byte buffer[256];
unsigned int length = 0; 

void setup() {
  byte pintype = INPUT;
  if (mode) {
    pintype = OUTPUT;
  }
  for (int i = 0; i<npins; i++) {
    pinMode(pins[i], pintype);
  }
  Serial.begin(9600);
  Serial.println("we gaming");
  if (mode) {
    Serial.println("deze arduino zend");
  } else {
    Serial.println("deze arduino ontvangt");
  }
}

void loop() {
  if (mode) {
    while(true) schrijf();
  } else {
    while(true) lees();
  }
}

void lees() {
  bool bits[8];
  for (int i = 0; i<npins; i++) {
    bits[i] = digitalRead(pins[i]);
  }

  char karakter = 0b0;
  for (int i = 0; i<npins; i++) {
    karakter |= (bits[i]<<i);
  }

  if (karakter == 0x02) {
    // detecteer start of text
    reading = 1;

  } else if (karakter == 0x03) {
     // detecteer end of text
    reading = 0;

    // print buffer
    for (int i=0; i<length; i++) {
      char letter = buffer[i];
      Serial.print(letter);
    }
    Serial.println();

    // leeg buffer
    for (int i=0; i<length; i++) {
      buffer[i] = 0x0;
    }
    length=0;

  } else if(reading) {
    Serial.print("0b");
    for (int i = 0; i<npins; i++) {
      Serial.print(bits[i]);
    }
    Serial.println();
  
    buffer[length+1] = karakter;
    length++;
  }

  delay(15);
}

void schrijf() {
  String tekst = Serial.readString();

  if (tekst.length() >= 1) {
    // schrijf start of text
    schrijfByte(0x2);

    // haal alle karakters uit de string
    for (int i=0; i<tekst.length(); i++) {
      char karakter = tekst.charAt(i);
      schrijfByte(karakter);
    }

    // schrijf end of text
    schrijfByte(0x3);
  }

  delay(100);

  /*
  for (int i = 0; i<npins; i++) {
    digitalWrite(pins[i], 0);
  }

  delay(1000);
  */
}

void schrijfByte(byte karakter) {
  // maak array met bits
  bool bits[8];

  // verplaats bits uit byte naar de array
  for (int i = 0; i<npins; i++) {
    bits[i] = karakter & (0b1 << i);
  }

  // print voor debuggen
  Serial.print("0b");
  for (int i = 0; i<npins; i++) {
    Serial.print(bits[i]);
  }
  Serial.println();

  // schrijf bits naar pinnetjes
  for (int i = 0; i<npins; i++) {
    digitalWrite(pins[i], bits[i]);
  }

  // houd de lijn voor 10ms aan
  delay(10);

  // zet alle pinnetjes weer uit
  for (int i = 0; i<npins; i++) {
    digitalWrite(pins[i], 0);
  }

  // wacht even 5ms voor een volgend karakter
  delay(5);
}