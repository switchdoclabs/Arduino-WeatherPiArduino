// Filename: LocalExample.h 
// Version 1.6 08/06/13 JS MiloCreek

// prototypes

void ExecuteLocalACTION_BUTTON_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject);
void ExecuteLocalTEXT_DISPLAY_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject);

void Local(char returnJSON[], char *buffer, currentObjectStructure *parsedObject)
{
#ifdef LOCALDEBUG
  printObject(parsedObject, "In LocalExample.h");
#endif
   switch (atol(parsedObject->ObjectType))
    {
        
        case FEEDBACK_ACTION_BUTTON_UITYPE:    
        case ACTION_BUTTON_UITYPE:
        {
           ExecuteLocalACTION_BUTTON_UITYPE(returnJSON, returnJSON, parsedObject);
        }
        break;
 
 
        case TEXT_DISPLAY_UITYPE:
        {
          ExecuteLocalTEXT_DISPLAY_UITYPE(returnJSON, returnJSON, parsedObject);
        }
        break;
    }
   
}


// Local Execute files


void ExecuteLocalACTION_BUTTON_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject)
{
#ifdef LOCALDEBUG  
     Serial.println("ExecuteACTION_BUTTON_UITYPE");
#endif

  // B-3 does a toggle on the LED on pin 13 (like B-1 does!!!)
  if (strcmp(parsedObject->ObjectServerID, "B-3") == 0)  
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
  
  // Note that Local objects DO NOT RETURN an Invalid Response.  That is done by the ExecuteJSONCommand function
  // returnJSONInValidResponse(returnJSON, returnJSON, parsedObject);

}


void ExecuteLocalTEXT_DISPLAY_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject)
{
#ifdef LOCALDEBUG
  Serial.println("ExecuteLocalTEXT_DISPLAY_UITYPE");
#endif

  // LT-5 returns a greeting message
  if (strcmp(parsedObject->ObjectServerID, "LT-5") == 0)  
  {
    if (strcmp(parsedObject->Validate, "YES") == 0)
    {
      // send validated response
      returnJSONValidateResponse(returnJSON, returnJSON, parsedObject);
    
      strcpy(returnJSON, buffer);
      return;
    }

    char responseData[40];


    sprintf(responseData,   "0, Hello from Local, Greetings",  messageCount);       
 
    returnJSONResponse(returnJSON, returnJSON, parsedObject, responseData );
       
    strcpy(returnJSON, buffer);
    return;
  }
  // Note that Local objects DO NOT RETURN an Invalid Response.  That is done by the ExecuteJSONCommand function
  // returnJSONInValidResponse(returnJSON, returnJSON, parsedObject);
}
  
