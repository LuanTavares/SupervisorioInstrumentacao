float tmp = 1;
float sinal = 0.25;

void setup()
{
  Serial.begin(9600);          //  setup serial  
}

void loop()
{
  
  tmp = tmp + sinal;
  if (tmp > 4.5) {
      sinal = sinal * -1;
  } else if (tmp < 1) {
      sinal = sinal * -1;
  }

  Serial.print("[");
  Serial.print(tmp);
  Serial.print("]"); 


  delay(100);     
}

