#include <SoftwareSerial.h>
#include <ArduinoJson.h>  //For Creating a Json File
DynamicJsonBuffer jsonBuffer;                                 //Set Buffer size to Dynamic
JsonObject& root = jsonBuffer.createObject();                //Create an object 'root' which is called later to print JSON Buffer      

char port[]="80";       // PORT Connected on
char aux_str[100];
 
String getStr="";

String AccessToken ="LIbbKy2lXHFEVG3kfYol";    //For Device 1
String data="";

String url="demo.thingsboard.io";
SoftwareSerial SIM900(8,9); //rx,tx

void setup() {

SIM900.begin(9600);
Serial.begin(9600);
delay(5000);
Serial.flush();
SIM900.flush();
Serial.println(0);
gprs_config1();
  
}

void loop() {
  gprs_config2();
  // put your main code here, to run repeatedly:

}

void gprs_config1(){
  Serial.println("Testing Stage 1");
  commandx("AT",2000);
  commandx("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"",2500);
  commandx("AT+SAPBR=3,1,\"APN\",\"airtelgprs.com\"",2500);
 // commandx("AT+CSTT=\"airtelgprs.com\",\"\",\"\"",3000);
  //commandx("AT+CIPMUX=0",2500);
  //commandx("AT+HTTPTERM",1000);
  //commandx("AT+CIPSTATUS",500);
  commandx("AT+SAPBR=0,1",5000);
  commandx("AT+SAPBR=1,1",10000);
  commandx("AT+CIPSTATUS",500);
  commandx("AT+SAPBR=2,1",4000);
  commandx("AT+HTTPINIT",3000);
  commandx("AT+HTTPPARA=\"CID\",1",2500);
  commandx("AT+HTTPPARA=\"URL\","+ url,3000);
}

void commandx(String CMD,int timer){
Serial.println("Sending Command : " + CMD);
SIM900.println(CMD);
delay(timer);
ShowSerialData();
delay(timer/3);  
  }

  
void ShowSerialData()
{
  while(SIM900.available()!=0)  /* If data is available on serial port */
  Serial.write(char (SIM900.read())); /* Print character received on to the serial monitor */
}


void gprs_config2(){
 
Serial.println("Testing Stage 2");  

   snprintf(aux_str, sizeof(aux_str), "AT+CIPSTART=\"TCP\",\"%s\",\"%s\"",url, port); //IP_address
   if (sendATcommand2(aux_str, "CONNECT OK", "CONNECT FAIL", 30000) == 1)
                 {      
                    Serial.println("Connected");
                    String json="";
                    root.printTo(json);                       //Store JSON in a String named 'json'
                    lengthOfJSON = json.length();             //This gives us total size of our json text
                    
                    //TCP packet to send POST Request on https (Thingsboard)
                      getStr="POST /api/v1/"+ AccessToken +"/telemetry HTTP/1.1\r\nHost: demo.thingsboard.io\r\nAccept: */*\r\nContent-Type: application/json\r\nContent-Length:"+lengthOfJSON+"\r\n\r\n"+json; 
                    //TCP packet to send POST Request on https (Thingsboard)
                 
                    String sendcmd = "AT+CIPSEND="+ String(getStr.length());
                          
                    if (sendATcommand2(sendcmd, ">", "ERROR", 10000) == 1)    
                    {
                      delay(100);
                      sendATcommand2(getStr, "SEND OK", "ERROR", 10000);      //Sending Data Here
                    }
                    Serial.println("Closing the Socket............");                            
                    sendATcommand2("AT+CIPCLOSE", "CLOSE OK", "ERROR", 10000);
                  }
                  else
                  {
                    Serial.println("Error opening the connection");
                  }
}
  
  //commandx("AT+HTTPACTION=0",5000);
  //commandx("AT+HTTPREAD",7000);
  //commandx("AT+HTTPTERM",2500);

}

int8_t sendATcommand2(String ATcommand, char* expected_answer1,char* expected_answer2, unsigned int timeout)
{
    uint8_t x=0,  answer=0;
    char response[3000];
    unsigned long previous;
    memset(response, '\0', 3000);    // Initialize the string
    delay(100);
    
    while( Serial1.available() > 0) Serial1.read();    // Clean the input buffer

    Serial1.println(ATcommand);    // Send the AT command 
    Serial.println(ATcommand);
    x = 0;
    previous = millis();

    // this loop waits for the answer
    do
    {
        // if there are data in the UART input buffer, reads it and checks for the asnwer
        if(Serial1.available() != 0)
        {    
            response[x] = Serial1.read();
            Serial.print(response[x]);
            x++;
            // check if the desired answer 1  is in the response of the module
            if (strstr(response, expected_answer1) != NULL)    
            {
                answer = 1;
            }
            // check if the desired answer 2 is in the response of the module
            else if (strstr(response, expected_answer2) != NULL)    
            {
                answer = 2;
            }
        }//if()
    
    }//do
    
    // Waits for the asnwer with time out
    while((answer == 0) && ((millis() - previous) < timeout));    
    return answer;
}//sendATcommand2()

