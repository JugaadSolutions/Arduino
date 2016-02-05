

void setup() {                

    DDRB = B10000000 ;
    DDRB = DDRB | B10000000;
    PORTB = B10000000;
    
    DDRC = B11111111 ;
    DDRC = DDRC | B11111111;
    PORTC = B11111111;    
    
    DDRA = B11111111 ;
    DDRA = DDRA | B11111111;
    PORTA = B11111111;  
}


void loop() {
  
  PORTB = B10000000;

  PORTC = 0xAA;
  PORTA = 0xAA;
  delay(1000); 
  PORTB = B00000000;

  PORTC = 0x55;  
  PORTA = 0x55;
  delay(1000);       
}
