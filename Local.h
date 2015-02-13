// Filename: Local.h 
// Version 1.0  07/29/14 SwitchDoc Labs

// prototypes


void ExecuteLocalACTION_BUTTON_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject);
void ExecuteLocalTEXT_DISPLAY_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject);
void ExecuteLocalSPEEDOMETER_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject);

void ExecuteLocalSOUND_ALARM_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject);
void ExecuteLocalSIMPLE_GAUGE_VIEW_LIVE_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject);
void ExecuteLocalSIMPLE_LINE_GRAPH_LIVE_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject);
void ExecuteLocalCOMPLEX_GAUGE_VIEW_LIVE_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject);
void ExecuteLocalBUBBLE_TABLE_VIEW_LIVE_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject);
void ExecuteLocalACTIVITY_INDICATOR_VIEW_LIVE_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject);
void ExecuteLocalSLIDER_LIVE_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject);
void ExecuteLocalSPARKLINE_LIVE_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject);
void ExecuteLocalSPARKLINE_DYNAMIC_LIVE_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject);


void Local(char returnJSON[], char *buffer, currentObjectStructure *parsedObject)
{
#ifdef LOCALDEBUG
  printObject(parsedObject, "In Local.h");
#endif
   switch (atol(parsedObject->ObjectType))
    {
        
        case FEEDBACK_ACTION_BUTTON_UITYPE:    
        case ACTION_BUTTON_UITYPE:
        {
           ExecuteLocalACTION_BUTTON_UITYPE(returnJSON, returnJSON, parsedObject);
        }
        break;
        
        case BUBBLE_TABLE_VIEW_LIVE_UITYPE:
        {
          ExecuteLocalBUBBLE_TABLE_VIEW_LIVE_UITYPE(returnJSON, returnJSON, parsedObject);
        }
        break;
        
        case SPEEDOMETER_UITYPE:
        {
          ExecuteLocalSPEEDOMETER_UITYPE(returnJSON, returnJSON, parsedObject);
        }
        break;
        
        case ACTIVITY_INDICATOR_VIEW_LIVE_UITYPE:
        {
          ExecuteLocalACTIVITY_INDICATOR_VIEW_LIVE_UITYPE(returnJSON, returnJSON, parsedObject);
        }
        break;
       
        case  SIMPLE_LINE_GRAPH_LIVE_UITYPE:
        {
          ExecuteLocalSIMPLE_LINE_GRAPH_LIVE_UITYPE(returnJSON, returnJSON, parsedObject);
        }
         case  TEXT_DISPLAY_UITYPE:
        {
          ExecuteLocalTEXT_DISPLAY_UITYPE(returnJSON, returnJSON, parsedObject);
        }
        
  
        
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
    strcpy(bubbleStatus, "LED 13 Flashed\0");
    returnJSONResponse(returnJSON, returnJSON, parsedObject, "OK" );
       
    strcpy(returnJSON, buffer);
    return;
  }
  
  // Note that Local objects DO NOT RETURN an Invalid Response.  That is done by the ExecuteJSONCommand function
  // returnJSONInValidResponse(returnJSON, returnJSON, parsedObject);

}


void ExecuteLocalBUBBLE_TABLE_VIEW_LIVE_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject)
{
#ifdef LOCALDEBUG
  Serial.println("ExecuteLocalBUBBLE_TABLE_VIEW_LIVE_UITYPE");
#endif

  // BTL-1 sends a message to the bubble screen
  if (strcmp(parsedObject->ObjectServerID, "BTL-1") == 0)  
  {
    if (strcmp(parsedObject->Validate, "YES") == 0)
    {
      // send validated response
      returnJSONValidateResponse(returnJSON, returnJSON, parsedObject);
    
      strcpy(returnJSON, buffer);
      return;
    }

    char responseData[40];

   
    sprintf(responseData,   bubbleStatus,  messageCount);       
 
    returnJSONResponse(returnJSON, returnJSON, parsedObject, responseData );
    strcpy(bubbleStatus, "");
    strcpy(returnJSON, buffer);
    return;
  }
  // Note that Local objects DO NOT RETURN an Invalid Response.  That is done by the ExecuteJSONCommand function
  // returnJSONInValidResponse(returnJSON, returnJSON, parsedObject);
}

extern float currentWindSpeed;
void ExecuteLocalSPEEDOMETER_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject)
{

  
    // M-3 returns the current wind speed
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
    char floatString[15];
    floatString[0] = '\0';

    dtostrf(currentWindSpeed,6,2,floatString);
    // returns value on analog channel 5
    sprintf(responseData, "%s", floatString);   
    
    
     
   
    returnJSONResponse(returnJSON, returnJSON, parsedObject, responseData );
       
    strcpy(returnJSON, buffer);
    return;
  }
  
}

void ExecuteLocalACTIVITY_INDICATOR_VIEW_LIVE_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject)
{

  
    // M-3 returns the current wind speed
  if (strcmp(parsedObject->ObjectServerID, "AIL-1") == 0)  
  {
    if (strcmp(parsedObject->Validate, "YES") == 0)
    {
      // send validated response
      returnJSONValidateResponse(returnJSON, returnJSON, parsedObject);
    
      strcpy(returnJSON, buffer);
      return;
    }

    char responseData[40];
 
    sprintf(responseData, "%s", "YES");   
    
    
     
   
    returnJSONResponse(returnJSON, returnJSON, parsedObject, responseData );
       
    strcpy(returnJSON, buffer);
    return;
  }
  
}

void ExecuteLocalTEXT_DISPLAY_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject)
{

  
    // LT-1 minmax of wind speed
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

    char floatStringMin[15];
    char floatStringMax[15];
    floatStringMin[0] = '\0';
    floatStringMax[0] = '\0';

    dtostrf(windSpeedMin,6,2,floatStringMin);
    dtostrf(windSpeedMax,6,2,floatStringMax);

    sprintf(responseData, "%s/%s", floatStringMin, floatStringMax);      
     
   
    returnJSONResponse(returnJSON, returnJSON, parsedObject, responseData );
       
    strcpy(returnJSON, buffer);
    return;
  }
  

     // LT-2 minmax of wind gusts
  if (strcmp(parsedObject->ObjectServerID, "LT-2") == 0)  
  {
    if (strcmp(parsedObject->Validate, "YES") == 0)
    {
      // send validated response
      returnJSONValidateResponse(returnJSON, returnJSON, parsedObject);
    
      strcpy(returnJSON, buffer);
      return;
    }

    char responseData[40];

    char floatStringMin[15];
    char floatStringMax[15];
    floatStringMin[0] = '\0';
    floatStringMax[0] = '\0';

    dtostrf(windGustMin,6,2,floatStringMin);
    dtostrf(windGustMax,6,2,floatStringMax);

    sprintf(responseData, "%s/%s", floatStringMin, floatStringMax);      
     
   
    returnJSONResponse(returnJSON, returnJSON, parsedObject, responseData );
       
    strcpy(returnJSON, buffer);
    return;
  }
     // LT-3 minmax of wind Direction
  if (strcmp(parsedObject->ObjectServerID, "LT-3") == 0)  
  {
    if (strcmp(parsedObject->Validate, "YES") == 0)
    {
      // send validated response
      returnJSONValidateResponse(returnJSON, returnJSON, parsedObject);
    
      strcpy(returnJSON, buffer);
      return;
    }

    char responseData[40];

    char floatStringMin[15];
    char floatStringMax[15];
    floatStringMin[0] = '\0';
    floatStringMax[0] = '\0';

    dtostrf(windDirectionMin,6,2,floatStringMin);
    dtostrf(windDirectionMax,6,2,floatStringMax);

    sprintf(responseData, "%s/%s", floatStringMin, floatStringMax);      
     
   
    returnJSONResponse(returnJSON, returnJSON, parsedObject, responseData );
       
    strcpy(returnJSON, buffer);
    return;
  }
  
       // LT-4 Total Rain
  if (strcmp(parsedObject->ObjectServerID, "LT-4") == 0)  
  {
    if (strcmp(parsedObject->Validate, "YES") == 0)
    {
      // send validated response
      returnJSONValidateResponse(returnJSON, returnJSON, parsedObject);
    
      strcpy(returnJSON, buffer);
      return;
    }

    char responseData[40];

    char floatStringMin[15];

    floatStringMin[0] = '\0';


    dtostrf(rainTotal,6,2,floatStringMin);


    sprintf(responseData, "%s", floatStringMin);      
     
   
    returnJSONResponse(returnJSON, returnJSON, parsedObject, responseData );
       
    strcpy(returnJSON, buffer);
    return;
  }
}


void ExecuteLocalSIMPLE_LINE_GRAPH_LIVE_UITYPE(char returnJSON[], char *buffer, currentObjectStructure *parsedObject)
{

  
    // SLGL-1 returns the current wind speed graph
  if (strcmp(parsedObject->ObjectServerID, "SLGL-1") == 0)  
  {
    if (strcmp(parsedObject->Validate, "YES") == 0)
    {
      // send validated response
      returnJSONValidateResponse(returnJSON, returnJSON, parsedObject);
    
      strcpy(returnJSON, buffer);
      return;
    }

    char responseData[200];
 
    sprintf(responseData, "%s", windSpeedBuffer);   
    
    
     
   
    returnJSONResponse(returnJSON, returnJSON, parsedObject, responseData );
       
    strcpy(returnJSON, buffer);
    return;
  }
     // SLGL-2 returns the current wind gust graph
  if (strcmp(parsedObject->ObjectServerID, "SLGL-2") == 0)  
  {
    if (strcmp(parsedObject->Validate, "YES") == 0)
    {
      // send validated response
      returnJSONValidateResponse(returnJSON, returnJSON, parsedObject);
    
      strcpy(returnJSON, buffer);
      return;
    }

    char responseData[200];
 
    sprintf(responseData, "%s", windGustBuffer);   
    
    
     
   
    returnJSONResponse(returnJSON, returnJSON, parsedObject, responseData );
       
    strcpy(returnJSON, buffer);
    return;
  }
     // SLGL-1 returns the current total rain graph
  if (strcmp(parsedObject->ObjectServerID, "SLGL-3") == 0)  
  {
    if (strcmp(parsedObject->Validate, "YES") == 0)
    {
      // send validated response
      returnJSONValidateResponse(returnJSON, returnJSON, parsedObject);
    
      strcpy(returnJSON, buffer);
      return;
    }

    char responseData[200];
 
    sprintf(responseData, "%s", windDirectionBuffer);   
    
    
     
   
    returnJSONResponse(returnJSON, returnJSON, parsedObject, responseData );
       
    strcpy(returnJSON, buffer);
    return;
  }
     // SLGL-1 returns the current vibration graph
  if (strcmp(parsedObject->ObjectServerID, "SLGL-4") == 0)  
  {
    if (strcmp(parsedObject->Validate, "YES") == 0)
    {
      // send validated response
      returnJSONValidateResponse(returnJSON, returnJSON, parsedObject);
    
      strcpy(returnJSON, buffer);
      return;
    }

    char responseData[200];
 
    sprintf(responseData, "%s", windSpeedBuffer);   
    
    
     
   
    returnJSONResponse(returnJSON, returnJSON, parsedObject, responseData );
       
    strcpy(returnJSON, buffer);
    return;
  }
  
}
