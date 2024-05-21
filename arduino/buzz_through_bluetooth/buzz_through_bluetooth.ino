char Incoming_value = 0;
const int buzzerPin = 12;  // Define the digital pin connected to the buzzer
bool buzzerOn = false;     // Flag to indicate if the buzzer is on or off

// Define the frequency for the alarm sound
const int alarmFrequency = 25000; // Adjust as needed for desired frequency

void setup() 
{
  Serial.begin(9600);         
  pinMode(buzzerPin, OUTPUT);   // Set the buzzer pin as an output
}

void loop()
{
  if(Serial.available() > 0)  
  {
    Incoming_value = Serial.read();      
    Serial.print(Incoming_value);        
    Serial.print("\n");        
    if(Incoming_value == '1') {            
      buzzerOn = true;                   // Turn on buzzer
    } else if(Incoming_value == '0') {       
      buzzerOn = false;                  // Turn off buzzer
      digitalWrite(buzzerPin, LOW);     // Ensure buzzer is off
    }
  }                            
  
  // Check if the buzzer is supposed to be on
  if (buzzerOn) {
    // Generate the square wave for the alarm sound
    tone(buzzerPin, alarmFrequency);  // Generate sound of specified frequency
  } else {
    noTone(buzzerPin);  // Stop generating tone
  }
}
