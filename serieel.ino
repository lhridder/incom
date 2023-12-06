// pins
unsigned int datapin = 8;
unsigned int controlpin = 9;

// maak de buffer incl lengte
byte buffer[256];
unsigned int length = 0; 

bool reading = 0;

void setup() {
  pinMode(datapin, INPUT);
  pinMode(controlpin, INPUT);

  // print debug informatie naar de seriele console
  Serial.begin(9600);
  Serial.println("we gaming");
  Serial.println("deze arduino zend en ontvangt");
}

void loop() {
  if(digitalRead(controlpin)==HIGH) {
    // is het het eerste karakter?
    if(reading) {
      read(0);
    } else {
      read(1);
    }

    // zet lees modus aan
    reading = 1;
  } else {
    // klaar met lezen, print buffer
    if (length >= 1) {
      print();
    }

    // kijk of er console data is
    if (Serial.available() >= 1) {
      Serial.println("we gaan wat sturen");
      schrijf(Serial.readString());
    }
  }
}

void read(bool wait) {
  // wacht bij eerste karakter
  if (wait) delay(5);

  // schrijf bit en debug
  bool bits[8];
  Serial.print("0b");
  for (int i = 0; i<8; i++) {
    bits[i] = digitalRead(datapin);
    Serial.print(bits[i]);

    // wacht 5ms tussen bits
    delay(5);
  }
  Serial.println();

  char karakter = 0b0;
  for (int i = 0; i<8; i++) {
    karakter |= (bits[i]<<i);
  }

  Serial.println(karakter);

  // sla het karakter op in de buffer
  buffer[length] = karakter;
  length++;

  // wacht 10ms tussen karakters
  delay(10);
}

void print() {
  // haal letters uit buffer
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

  reading = 0;

  Serial.println("klaar met lezen");
}

void schrijf(String tekst) {
  Serial.println("we gaan sturen");

  // zet claim pin aan
  pinMode(controlpin, OUTPUT);
  digitalWrite(controlpin, HIGH);

  // wacht 5ms met sturen
  delay(5);

  pinMode(datapin, OUTPUT);

  for (int i=0; i<tekst.length(); i++) {
    char karakter = tekst.charAt(i);

    /*
    // maak array met bits
    bool bits[8];

    // verplaats bits uit byte naar de array
    for (int i = 0; i<8; i++) {
      bits[i] = karakter & (0b1 << i);
    }

    // print voor debuggen
    Serial.print("0b");
    for (int i = 0; i<8; i++) {
      Serial.print(bits[i]);
    }
    Serial.println();
    */

    // schrijf bits naar pinnetjes
    for (int i = 0; i<8; i++) {
      digitalWrite(datapin, karakter & (0b1 << i));

      // houd de lijn voor 5ms aan
      delay(5);
    }

    digitalWrite(datapin, LOW);

    // wacht 10ms tussen karakters
    delay(10);
  }

  digitalWrite(controlpin, LOW);
  delay(10);

  pinMode(datapin, INPUT);
  pinMode(controlpin, INPUT);

  Serial.println("klaar met sturen");
}
