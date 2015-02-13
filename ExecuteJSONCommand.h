// Filename: ExecuteJSONCommand.h
// Version 1.6 09/11/13 RV MiloCreek
// Version 1.6 reload of Version 1.5

// define LED colors



#define LED_BLACK 0
#define LED_BLUE 1
#define LED_GREEN 2
#define LED_ORANGE 3
#define LED_PURPLE 4
#define LED_RED 5
#define LED_YELLOW 6

// RasPiConnectServer interface constants


#define REMOTE_WEBVIEW_UITYPE 1


#define ACTION_BUTTON_UITYPE 16
#define FEEDBACK_ACTION_BUTTON_UITYPE 17

#define SINGLE_LED_DISPLAY_UITYPE 32


#define SPEEDOMETER_UITYPE 64
#define VOLTMETER_UITYPE 128
#define BARMETER_UITYPE 129

#define SERVER_STATUS_UITYPE 256

#define PICTURE_REMOTE_WEBVIEW_UITYPE 512

#define LABEL_UITYPE 1024

#define FM_BLINK_LED_UITYPE 2048

#define TEXT_DISPLAY_UITYPE 4096

#define TOGGLE_SWITCH_UITYPE 33

#define SEND_TEXT_UITYPE 34

#define SOUND_ALARM_UITYPE 35

// Control Pack 1

// SF GaugeView
#define SIMPLE_GAUGE_VIEW_LIVE_UITYPE 37
// BEMSimpleLineGraph
#define SIMPLE_LINE_GRAPH_LIVE_UITYPE 38

// WMGaugeView
#define COMPLEX_GAUGE_VIEW_LIVE_UITYPE 40

// SHLineGraphView
#define COMPLEX_LINE_GRAPH_LIVE_UITYPE 42
// UIBubbleTableView
#define BUBBLE_TABLE_VIEW_LIVE_UITYPE 43

// MONActivityIndicator
#define ACTIVITY_INDICATOR_VIEW_LIVE_UITYPE 44

//ASValueTrackingSlider
#define SLIDER_LIVE_UITYPE 45

//FS3PlotStrip
#define  SPARKLINE_LIVE_UITYPE 46

#define  SPARKLINE_DYNAMIC_LIVE_UITYPE 47





#define FILE_DIRECTORY_CALL 8192
#define FILE_READ_CALL 16384
#define FILE_WRITE_CALL 32768

// prototypes


void ExecuteREMOTE_WEBVIEW_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject);
void ExecutePICTURE_REMOTE_WEBVIEW_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject);
void ExecuteACTION_BUTTON_UITYPE(char returnJSON[], char* buffer, currentObjectStructure *parsedObject);
void ExecuteTEXT_DISPLAY_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject); 
void ExecuteMETER_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject);
void ExecuteBARMETER_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject);
void ExecuteSINGLE_LED_DISPLAY_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject);
void ExecuteSERVER_STATUS_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject);
void ExecuteFM_BLINK_LED_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject);
void ExecuteSEND_TEXT_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject);



void returnJSONInValidResponse(char returnJSON[], char *buffer, currentObjectStructure *parsedObject);
void returnJSONValidateResponse(char returnJSON[], char *buffer, currentObjectStructure *parsedObject);
void returnJSONResponse(char returnJSON[], char *buffer, currentObjectStructure *parsedObject, char* response);
#ifdef DEBUG
void printObject(currentObjectStructure *parsedObject, char* location);
#endif
#ifdef LOCALDEBUG
void printObject(currentObjectStructure *parsedObject, char* location);
#endif



#include LOCALFILE
// main code

void ExecuteCommandAndReturnJSONString(char *JSONString, long messageCount, char md5str[], currentObjectStructure *parsedObject, char returnJSON[], char *buffer)
{
 
    

     
#ifdef DEBUG 
  Serial.println("Entering Executing Command And Return");
  Serial.println("------------------");
#endif
 
  read_commands(JSONString, parsedObject);
#ifdef DEBUG
  printObject(parsedObject, "After read_commands");
#endif
   
  
  
  if (strcmp(parsedObject->Password, md5str) != 0)
  {
    
    // Authentication fails
 #ifdef DEBUG   
   Serial.println("Authentication fails");
 #endif  
   
    sprintf(returnJSON, "{\"OBJECTTYPE\" : \"%s\" \"OBJECTID\" : \"%s\", \"ERROR\" : \"Username or Password Mismatch\" }\n", parsedObject->ObjectType, parsedObject->ObjectID);
   
    Serial.print("returnJSON=");
    Serial.println(returnJSON);
    Serial.print("/");
    Serial.print(strlen(returnJSON));
    Serial.println("/");
   // release_token_list(token_list);

     

   strcpy(returnJSON, buffer);


  }
  
#ifdef DEBUG
  Serial.println("Authentication passes");
  Serial.print("ObjectType=");
  Serial.println(parsedObject->ObjectType);
#endif
  // Authentication passes
  
  // now check for user local objects
  
 Local(returnJSON, returnJSON, parsedObject);
   
 if (strlen(returnJSON) == 0)  // if returnJSON still is zero length, then there are no matching local objects so try built in objects
 {

   switch (atol(parsedObject->ObjectType))
    {
            
        case REMOTE_WEBVIEW_UITYPE:
        {
          ExecuteREMOTE_WEBVIEW_UITYPE(returnJSON, returnJSON, parsedObject);        
        }
        break;
        
        case PICTURE_REMOTE_WEBVIEW_UITYPE:
        {
          ExecutePICTURE_REMOTE_WEBVIEW_UITYPE(returnJSON, returnJSON, parsedObject);        
        }
        break;
        
        case FEEDBACK_ACTION_BUTTON_UITYPE:    
        case ACTION_BUTTON_UITYPE:
        {
           ExecuteACTION_BUTTON_UITYPE(returnJSON, returnJSON, parsedObject);
        }
        break;
               
        case SINGLE_LED_DISPLAY_UITYPE:
        {
            
          ExecuteSINGLE_LED_DISPLAY_UITYPE(returnJSON, returnJSON, parsedObject);
            
        }
        break;
            
        case SPEEDOMETER_UITYPE:
        case VOLTMETER_UITYPE:
        {
          ExecuteMETER_UITYPE(returnJSON, returnJSON, parsedObject);
            
        }
        break;    
        
        case SERVER_STATUS_UITYPE:
        {
          ExecuteSERVER_STATUS_UITYPE(returnJSON, returnJSON, parsedObject);
        }
        break;
        
        case FM_BLINK_LED_UITYPE:
        {
          ExecuteFM_BLINK_LED_UITYPE(returnJSON, returnJSON, parsedObject);
        }
        break;
        
        case TEXT_DISPLAY_UITYPE:
        {
          ExecuteTEXT_DISPLAY_UITYPE(returnJSON, returnJSON, parsedObject);
        }
        break;
        
        case SEND_TEXT_UITYPE:
        {
             ExecuteSEND_TEXT_UITYPE(returnJSON, returnJSON, parsedObject);
        }
        break;
        
        case BARMETER_UITYPE:
        {
           ExecuteBARMETER_UITYPE(returnJSON, returnJSON, parsedObject);            
        }
        break;
  
       default:
       {
            returnJSONInValidResponse(returnJSON, returnJSON, parsedObject);
       }
       break;

            
            
    }
 

 }


    
    
  
  
#ifdef DEBUG 
   Serial.println("End Of Command And Return");

   Serial.print("returnJSON =");
   Serial.println(returnJSON);
#endif

     strcpy(returnJSON, buffer);
   
}

//
//
// helper functions
//
//

void returnJSONValidateResponse(char returnJSON[], char *buffer, currentObjectStructure *parsedObject)
{
     sprintf(returnJSON,            
           "{\"OBJECTTYPE\" : \"%s\", \"OBJECTID\" : \"%s\", \"OBJECTSERVERID\" : \"%s\", \"PASSWORD\" : \"%s\", \"OBJECTFLAGS\" : \"%s\", \"ARDUINOCONNECTVSERVERVERSIONNUMBER\" : \"%s\", \"RESPONSE\" : \"VALIDATED\" }\n",
           parsedObject->ObjectType, parsedObject->ObjectID, parsedObject->ObjectServerID, parsedObject->Password, parsedObject->ObjectFlags, VERSIONNUMBER);

     strcpy(returnJSON, buffer); 
}

void returnJSONInValidResponse(char returnJSON[], char *buffer, currentObjectStructure *parsedObject)
{
     sprintf(returnJSON,            
           "{\"OBJECTTYPE\" : \"%s\", \"OBJECTID\" : \"%s\", \"OBJECTSERVERID\" : \"%s\", \"PASSWORD\" : \"%s\", \"OBJECTFLAGS\" : \"%s\", \"ARDUINOCONNECTVSERVERVERSIONNUMBER\" : \"%s\", \"RESPONSE\" : \"NO\" }\n",
           parsedObject->ObjectType, parsedObject->ObjectID, parsedObject->ObjectServerID, parsedObject->Password, parsedObject->ObjectFlags, VERSIONNUMBER);

     strcpy(returnJSON, buffer); 
}

void returnJSONResponse(char returnJSON[], char *buffer, currentObjectStructure *parsedObject, char* response)
{
#ifdef DEBUG  
    Serial.print("Control response=");
    Serial.println(response);
#endif

     sprintf(returnJSON,            
           "{\"OBJECTTYPE\" : \"%s\", \"OBJECTID\" : \"%s\", \"OBJECTSERVERID\" : \"%s\", \"PASSWORD\" : \"%s\", \"OBJECTFLAGS\" : \"%s\", \"ARDUINOCONNECTVSERVERVERSIONNUMBER\" : \"%s\", \"RESPONSE\" : \"%s\" }\n",
           parsedObject->ObjectType, parsedObject->ObjectID, parsedObject->ObjectServerID, parsedObject->Password, parsedObject->ObjectFlags, VERSIONNUMBER, response);

     strcpy(returnJSON, buffer);
  
}

#ifdef DEBUG
void printObject(currentObjectStructure *parsedObject, char* location)
{
    Serial.println("-----------");
    Serial.println(location);
    Serial.println("-----------");
    Serial.print("ObjectID=");
    Serial.println(parsedObject->ObjectID);
    Serial.print("ObjectServerID=");
    Serial.println(parsedObject->ObjectServerID);
    Serial.print("Password=");
    Serial.println(parsedObject->Password);
    Serial.print("ObjectType=");
    Serial.println(parsedObject->ObjectType);
    Serial.print("Validate=");
    Serial.println(parsedObject->Validate);
    Serial.print("ObjectFlags=");
    Serial.println(parsedObject->ObjectFlags);
    Serial.print("ObjectAction=");
    Serial.println(parsedObject->ObjectAction);
    Serial.print("ObjectName=");
    Serial.println(parsedObject->ObjectName);
    Serial.println("-----------");
}
#endif

#ifdef LOCALDEBUG
#ifndef DEBUG
void printObject(currentObjectStructure *parsedObject, char* location)
{
    Serial.println("-----------");
    Serial.println(location);
    Serial.println("-----------");
    Serial.print("ObjectID=");
    Serial.println(parsedObject->ObjectID);
    Serial.print("ObjectServerID=");
    Serial.println(parsedObject->ObjectServerID);
    Serial.print("Password=");
    Serial.println(parsedObject->Password);
    Serial.print("ObjectType=");
    Serial.println(parsedObject->ObjectType);
    Serial.print("Validate=");
    Serial.println(parsedObject->Validate);
    Serial.print("ObjectFlags=");
    Serial.println(parsedObject->ObjectFlags);
    Serial.print("ObjectAction=");
    Serial.println(parsedObject->ObjectAction);
    Serial.print("ObjectName=");
    Serial.println(parsedObject->ObjectName);
    Serial.println("-----------");
}
#endif
#endif

void initParsedObject(currentObjectStructure *parsedObject)
{
    
    parsedObject->ObjectID[0] = '\0';
    parsedObject->ObjectServerID[0] = '\0';
    parsedObject->Password[0] = '\0';
    parsedObject->ObjectType[0] = '\0';
    parsedObject->Validate[0] = '\0';
    parsedObject->ObjectFlags[0] = '\0';
    parsedObject->ObjectAction[0] = '\0';
    parsedObject->ObjectName[0] = '\0';
}


//
//
// Control Functions
//
//

void ExecuteACTION_BUTTON_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject)
{
#ifdef DEBUG  
     Serial.println("ExecuteACTION_BUTTON_UITYPE");
#endif

  // B-1 does a toggle on the LED on pin 13
  if (strcmp(parsedObject->ObjectServerID, "B-1") == 0)  
  {
    if (strcmp(parsedObject->Validate, "YES") == 0)
    {
      // send validated response
      returnJSONValidateResponse(returnJSON, returnJSON, parsedObject);
    
      strcpy(returnJSON, buffer);
      return;
    }
    
#define led 13

    pinMode(led, OUTPUT);
    digitalWrite(led, HIGH);
    delay(500);
    digitalWrite(led, LOW);
   
    returnJSONResponse(returnJSON, returnJSON, parsedObject, "OK" );
       
    strcpy(returnJSON, buffer);
    return;
  }
  
  // FB-1 just does a toggle from on to off from the button name

  if (strcmp(parsedObject->ObjectServerID, "FB-1") == 0)  
  {

    if (strcmp(parsedObject->Validate, "YES") == 0)
    {
      // send validated response
      returnJSONValidateResponse(returnJSON, returnJSON, parsedObject);
      strcpy(returnJSON, buffer);
      return;
    }
 
     String myString;
     myString = parsedObject->ObjectName;
     
     char responseData[40];
     

     myString.toLowerCase();

     if (myString.indexOf(" off") > 0)
     {
       myString.replace(" off", " on");
       myString.toCharArray(responseData, 40);
     }
     else
       if (myString.indexOf(" on") > 0)
       {

          myString.replace(" on", " off");
          myString.toCharArray(responseData, 40);
        }
        else
        {
          strcpy(parsedObject->ObjectName,responseData);
         }
                
      returnJSONResponse(returnJSON, returnJSON, parsedObject, responseData );
  
      strcpy(returnJSON, buffer);
      return;

  }
  
  
  returnJSONInValidResponse(returnJSON, returnJSON, parsedObject);
}


void ExecuteTEXT_DISPLAY_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject)
{
#ifdef DEBUG
  Serial.println("ExecuteTEXT_DISPLAY_UITYPE");
#endif

  // LT-1 returns total count of web requests 
  if (strcmp(parsedObject->ObjectServerID, "LT-1") == 0)  
  {
    if (strcmp(parsedObject->Validate, "YES") == 0)
    {
      // send validated response
      returnJSONValidateResponse(returnJSON, returnJSON, parsedObject);
    
      strcpy(returnJSON, buffer);
      return;
    }

    char responseData[40];


    sprintf(responseData,   "0, %d, Web Requests",  messageCount);       
 
    returnJSONResponse(returnJSON, returnJSON, parsedObject, responseData );
       
    strcpy(returnJSON, buffer);
    return;
  }
    returnJSONInValidResponse(returnJSON, returnJSON, parsedObject);
}
  
void ExecuteMETER_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject)
{
#ifdef DEBUG
  Serial.println("ExecuteMETER_UITYPE");
#endif

  // M-1 returns random voltage between 0.0 and 5.0
  if (strcmp(parsedObject->ObjectServerID, "M-1") == 0)  
  {
    if (strcmp(parsedObject->Validate, "YES") == 0)
    {
      // send validated response
      returnJSONValidateResponse(returnJSON, returnJSON, parsedObject);
    
      strcpy(returnJSON, buffer);
      return;
    }

    char responseData[40];
    float myNumber;
    long randNumber;
    
    // print a random number from 10 to 19
    randNumber = random(0, 50);
    myNumber = 5.0- (float)randNumber/10.0;
    
    dtostrf(myNumber, 3, 2, responseData);

     
   
    returnJSONResponse(returnJSON, returnJSON, parsedObject, responseData );
       
    strcpy(returnJSON, buffer);
    return;
  }
   // M-2 returns voltage on analog pin 0
  if (strcmp(parsedObject->ObjectServerID, "M-2") == 0)  
  {
    if (strcmp(parsedObject->Validate, "YES") == 0)
    {
      // send validated response
      returnJSONValidateResponse(returnJSON, returnJSON, parsedObject);
    
      strcpy(returnJSON, buffer);
      return;
    }

    char responseData[40];
    float myNumber;
    long randNumber;
    
    // returns value on analog channel 0
    sprintf(responseData, "%i", analogRead(0));
    
     
   
    returnJSONResponse(returnJSON, returnJSON, parsedObject, responseData );
       
    strcpy(returnJSON, buffer);
    return;
  }
  
    // M-3 returns voltage on analog pin 5
  if (strcmp(parsedObject->ObjectServerID, "M-3") == 0)  
  {
    if (strcmp(parsedObject->Validate, "YES") == 0)
    {
      // send validated response
      returnJSONValidateResponse(returnJSON, returnJSON, parsedObject);
    
      strcpy(returnJSON, buffer);
      return;
    }

    char responseData[40];

    
    
        // returns value on analog channel 5
    sprintf(responseData, "%i", analogRead(5));
   
   
    returnJSONResponse(returnJSON, returnJSON, parsedObject, responseData );
       
    strcpy(returnJSON, buffer);
    return;
  }
  
  
    returnJSONInValidResponse(returnJSON, returnJSON, parsedObject);
  
}
   
void ExecuteBARMETER_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject)
{
#ifdef DEBUG
  Serial.println("ExecuteBARMETER_UITYPE");
#endif
  // BR-1 returns random voltage between 0.0 and 10.0
  if (strcmp(parsedObject->ObjectServerID, "BR-1") == 0)  
  {
    if (strcmp(parsedObject->Validate, "YES") == 0)
    {
      // send validated response
      returnJSONValidateResponse(returnJSON, returnJSON, parsedObject);
    
      strcpy(returnJSON, buffer);
      return;
    }

    char responseData[40];
    float myNumber;
    long randNumber;
    
    // get a random number from 0 to 100
    randNumber = random(0, 100);
    myNumber = 10.0- (float)randNumber/10.0;
    
    dtostrf(myNumber, 3, 2, responseData);

     
   
    returnJSONResponse(returnJSON, returnJSON, parsedObject, responseData );
       
    strcpy(returnJSON, buffer);
    return;
  }
 
    returnJSONInValidResponse(returnJSON, returnJSON, parsedObject);
  
}
  
  
    
void ExecuteSINGLE_LED_DISPLAY_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject)
{
#ifdef DEBUG
  Serial.println("ExecuteSINGLE_LED_DISPLAY_UITYPE");
#endif
  // L-1 returns random voltage between 0.0 and 10.0
  if (strcmp(parsedObject->ObjectServerID, "L-1") == 0)  
  {
    if (strcmp(parsedObject->Validate, "YES") == 0)
    {
      // send validated response
      returnJSONValidateResponse(returnJSON, returnJSON, parsedObject);
    
      strcpy(returnJSON, buffer);
      return;
    }

    char responseData[40];
    float myNumber;
    long randNumber;
    
    // print a random number from 0 to 6
    randNumber = random(0, 6);
   
    sprintf(responseData, "%i", randNumber);
   
  
    returnJSONResponse(returnJSON, returnJSON, parsedObject, responseData );
       
    strcpy(returnJSON, buffer);
    return;
  }
 
    returnJSONInValidResponse(returnJSON, returnJSON, parsedObject);
  
}
  
  
      
void ExecuteSERVER_STATUS_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject)
{
#ifdef DEBUG
  Serial.println("ExecuteSERVER_STATUS_UITYPE");
#endif
  // SS-1 returns Status of Server
  if (strcmp(parsedObject->ObjectServerID, "SS-1") == 0)  
  {
    if (strcmp(parsedObject->Validate, "YES") == 0)
    {
      // send validated response
      returnJSONValidateResponse(returnJSON, returnJSON, parsedObject);
    
      strcpy(returnJSON, buffer);
      return;
    }

    char *responseData = "2";
 
    returnJSONResponse(returnJSON, returnJSON, parsedObject, responseData );
       
    strcpy(returnJSON, buffer);
    return;
  }
 
    returnJSONInValidResponse(returnJSON, returnJSON, parsedObject);
  
}
       
void ExecuteFM_BLINK_LED_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject)
{
#ifdef DEBUG
  Serial.println("ExecuteFM_BLINK_LED_UITYPE");
#endif

  // BL-1 sends a random flash rate back to client
  if (strcmp(parsedObject->ObjectServerID, "BL-1") == 0)  
  {
    if (strcmp(parsedObject->Validate, "YES") == 0)
    {
      // send validated response
      returnJSONValidateResponse(returnJSON, returnJSON, parsedObject);
    
      strcpy(returnJSON, buffer);
      return;
    }

    char responseData[40];
    float myNumber;
    long randNumber;
    
    // get a random number from 0 to 100
    randNumber = random(1, 9);
    myNumber = 1.0- (float)randNumber/10.0+0.1;
    char floatingNumber[10];
    
    dtostrf(myNumber, 3, 2, floatingNumber);
 
    sprintf(responseData, "%s, %s, Random Flash", floatingNumber, floatingNumber);
    returnJSONResponse(returnJSON, returnJSON, parsedObject, responseData );
       
    strcpy(returnJSON, buffer);
    return;
  }
 
    returnJSONInValidResponse(returnJSON, returnJSON, parsedObject);
  
}


void ExecuteSEND_TEXT_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject)
{
#ifdef DEBUG
  Serial.println("ExecuteSEND_TEXT_UITYPE");
#endif

  // ST-1 stores a string in a global variable (limited to 40 characters)
  if (strcmp(parsedObject->ObjectServerID, "ST-1") == 0)  
  {
    if (strcmp(parsedObject->Validate, "YES") == 0)
    {
      // send validated response
      returnJSONValidateResponse(returnJSON, returnJSON, parsedObject);
    
      strcpy(returnJSON, buffer);
      return;
    }
    
    strncpy(ST1Text, parsedObject->ObjectAction, 40);
    
    returnJSONResponse(returnJSON, returnJSON, parsedObject, "OK" );
       
    strcpy(returnJSON, buffer);
    return;
  }
 
    returnJSONInValidResponse(returnJSON, returnJSON, parsedObject);
  
}
  
  
  
void ExecuteREMOTE_WEBVIEW_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject)
{
#ifdef DEBUG
  Serial.println("ExecuteREMOTE_WEBVIEW_UITYPE");
#endif

  // Returns a sample web page back to Client
  // purposely small to save memory
  
  if (strcmp(parsedObject->ObjectServerID, "W-1") == 0)  
  {
    if (strcmp(parsedObject->Validate, "YES") == 0)
    {
      // send validated response
      returnJSONValidateResponse(returnJSON, returnJSON, parsedObject);
    
      strcpy(returnJSON, buffer);
      return;
    }
    
    char htmlReply1[] = "<HTML>Hello from Arduino.<BR>  Free Memory is: ";
    
    char htmlReply2[] = " bytes<BR>Last Send Text: ";
    char htmlReply3[] = "</HTML>";
 
    char fullResponse[strlen(htmlReply1)+strlen(htmlReply2)+6+strlen(ST1Text)+strlen(htmlReply3)];
    fullResponse[0] = '\0';
    strcat(fullResponse, htmlReply1);
    
    char myInt[12];
    itoa(freeMemory(), myInt, 10);
    strcat(fullResponse, myInt );
    
    strcat(fullResponse, htmlReply2);
    strcat(fullResponse, ST1Text);
    strcat(fullResponse, htmlReply3);
      
    returnJSONResponse(returnJSON, returnJSON, parsedObject, fullResponse );
       
    strcpy(returnJSON, buffer);
    return;
  }
 
    returnJSONInValidResponse(returnJSON, returnJSON, parsedObject);
  
}
 
   
void ExecutePICTURE_REMOTE_WEBVIEW_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject)
{
#ifdef DEBUG
  Serial.println("ExecuteREMOTE_WEBVIEW_UITYPE");
#endif

  // Returns a sample web page back to Client to show that a picture could be sent back
  // purposely small to save memory
  
  if (strcmp(parsedObject->ObjectServerID, "W-2") == 0)  
  {
    if (strcmp(parsedObject->Validate, "YES") == 0)
    {
      // send validated response
      returnJSONValidateResponse(returnJSON, returnJSON, parsedObject);
    
      strcpy(returnJSON, buffer);
      return;
    }
    
    char htmlReply1[] = "<HTML><BR><BR><BR>Hello from Arduino.<BR>  This could be a remote picture. ";
    
    char htmlReply2[] = "</HTML>";
    
    char fullResponse[strlen(htmlReply1)+strlen(htmlReply2)+6];
    fullResponse[0] = '\0';
    strcat(fullResponse, htmlReply1);
    strcat(fullResponse, htmlReply2);
    
    returnJSONResponse(returnJSON, returnJSON, parsedObject, fullResponse );
       
    strcpy(returnJSON, buffer);
    return;
  }
 
    returnJSONInValidResponse(returnJSON, returnJSON, parsedObject);
  
}
 
