// Filename: WebServer.h
// Version 1.5 08/19/13 RV MiloCreek

/* from small json page http://chriswarbo.net/index.php?page=cedi&type=misc&id=1%2F3%2F6%2F10 */

// Prototypes
int value_length(char* json);

void run_command(char* name, char* value, currentObjectStructure *parsedObject);
void run_read(char* value);
void run_write(char* value);
void run_mode(char* value);
void run_query(char* value);

int json_length(char* json) {
  // Give this a pointer to some JSON data and it will
  // return the length of that JSON.
  
  if (json == 0) {
    return 0;    // Null pointer
  }
  
  if (json[0] != '{') {
    return 0;    // Not JSON
  }

  // Now that we know we have a JSON object, we defer
  // the actual calculation to value_length
  return value_length(json);
}

int value_length(char* json) {
  // This is given a fragment of JSON and returns how
  // many characters it contains. This fragment might
  // be an object, a number, a string , etc.
  if (json == 0) {
    return 0;    // Null pointer
  }
  
  // Switch over each possibility
  int index = 0;
  switch (json[index]) {
    case '{':
        // This is a JSON object. Find the matching '}'
        do {
          index++;    // Read ahead
          if (json[index] == '"') {
            // Skip strings, as they may contain unwanted '}'
            index = index + value_length(json+index);
          }
          if (json[index] == '{') {
            // Recurse past nested objects
            index = index + value_length(json+index);
          }
        } while (json[index] != '}');
        return index + 1;    // Include the '{' and '}' in the length
    case '"':
      // This is a string. Scan ahead to the first unescaped '"'
      do {
        if (json[index] == '\\') {
          index++; // Skip escaped quotes
        }
        index++;    // Read ahead
      } while (json[index] != '"');
      return index+1;    // Include the quotes in the string's length
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '-':
      // We're a number. Loop forever until we find a non-number character.
      // Note, this is a simplistic parser that is equivalent to the regex
      // [0123456789-][0123456789.eE]* This allows malformed numbers like
      // 0.0.0.0e0.0e.0
      do {
        index++;
        switch (json[index]) {
          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
          case '.':
          case 'e':
          case 'E':
            break;    // Numeric
          default:
            return index;    // Non-numeric. Stop counting.
        }
      } while (1);
    default:
      // Unknown. Ignore it.
      return 0;
  }
}

void read_commands(char* json, currentObjectStructure *parsedObject) {
  // Takes a JSON string and looks for any commands it
  // contains. These are "key":value pairs, which are
  // sent as arguments to the "run_command" function as
  // they are encountered.
  int length = json_length(json);
  int index = 0;    // Used to loop through the contents
  int temp;    // Our parsing uses lookahead, this stores how far we've gone
  
  // Only bother doing something if json has some contents.
  // When this condition is false, it's essentially the
  // escape clause of our recursion.
  if (length > 2) {    // 2 == empty, since we have '{' and '}'
    index++;    // Skip past the '{' to get at the contents
    while (index < length) {
      switch (json[index]) {
        case ' ':
          // Whitespace is insignificant
          index++;
          break;
        case '{':
          // We have an object in an object, let's recurse
          read_commands(json+index, parsedObject );
          index = index + json_length(json+index);
          break;
        case '"':
          // A string. This should be part of a key:value pair
          if (index + 2 >= length) {
            // JSON can't end with an opening quote. Bail out.
            break;
          }
          
          // Look one character ahead, then keep going until
          // we find our matching close quote
          temp = index+1;
          while ((json[temp] != '"') && (temp < length)) {
            // We've not found our close quote, so look ahead
            if (json[temp] == '\\') {
              // Increment twice to skip over escaped characters
              temp++;
            }
            temp++;
          }
          if (temp >= length-2) {
            // We've reached the end of the JSON without finding
            // a close quote. Bail out.
            break;
          }
          
          // Now we've read our name, find our associated value
          temp++;    // It must start after the close quote
          while ((json[temp] == ' ') && (temp < length)) {
            temp++;    // Skip whitespace
          }
          if (json[temp] != ':') {
            // We must have a colon between the name and the value
            // Bail out if not
            break;
          }
          temp++;    // We don't need the colon, skip it
          while ((json[temp] == ' ') && (temp < length)) {
            temp++;    // Skip whitespace
          }
          
          // Wherever we are, we must have found our value
          // Tell run_command what we've found
          run_command(json+index, json+temp, parsedObject);
          
          // Now let's get our parser ready for the next value
          index = temp + value_length(json+temp);    // Skip the value
          while ((json[index] == ' ') && (index < length)) {
            index++;    // Skip whitespace
          }
          if (json[index] == ',') {
            // Skip commas between name:value pairs
            index++;
          }
          break;    // Done
        default:
          // Unknown input. Oops.
          index++;
      }
    }
  }
  else {
    // Our JSON is empty
    return;
  }
}

short compare_strings(char* string1, char* string2) {
  // Compare the first character array, which is not
  // null-terminated and is in quotes, to the second
  // which can be null-terminated and without quotes
  int first_size = value_length(string1);
  int second_size;
  for (second_size = 0; string2[second_size] != '\0'; second_size++) {
    // Do nothing. The loop parameters count the string for us.
  }
  
  // first_size includes quotes, so we don't include them
  // in our check
  if (first_size - 2 != second_size) {
    // The size is different, so the strings are different
    return 0;
  }
  
  // Now do a lexicographical comparison
  int index;
  for (index = 0; index < first_size - 2; index++) {
    if (string1[index+1] != string2[index]) {
      return 0;    // Mismatch
    }
  }
  
  // If we're here then our tests couldn't find any different
  return 1;
}

float compile_digits(char* value) {
  // Reads the JSON number in value and returns its value as
  // a float.
  // NOTE: This will overrun the digits by 1! Make sure that
  // the pointer doesn't end when the digits do, or this will
  // produce unspecified behaviour. This shouldn't be an
  // issue if your pointer is part of some JSON object, since
  // we can always overrun on to the '}' in the worst case.
  float result = 0.0;    // We build this up from the whole numbers
  float fraction_result = 0.0;    // We build this up from the fraction
  int value_size = value_length(value);
  int index;
  short fractional = 0;    // 0 = no decimal point found, 1 = found decimal point
  for (index = 0; index < value_size; index++) {
    result = result * 10.0;    // Bump up the magnitude for each digit
    switch (value[index]) {
      // Each digit falls through the appropriate number of
      // increments before breaking at 0.
      case '9':
        result = result + 1.0;
      case '8':
        result = result + 1.0;
      case '7':
        result = result + 1.0;
      case '6':
        result = result + 1.0;
      case '5':
        result = result + 1.0;
      case '4':
        result = result + 1.0;
      case '3':
        result = result + 1.0;
      case '2':
        result = result + 1.0;
      case '1':
        result = result + 1.0;
      case '0':
        break;
      case '-':
        result = result * -1.0;
        break;
      case '.':
        fractional = 1;    // Remember the decimal
        result = result / 10.0;    // Undo the magnitude bump
        index = value_size;    // Break out of the for loop
        break;
      case 'e':
      case 'E':
        // Not yet handled
      default:
        break;
    }
  }
  if (fractional) {
    // We found a decimal point, so we need to build up
    // the fraction too. Easiest way is right-to-left.
    for (index = value_size; index >= 0; index--) {
      fraction_result = fraction_result / 10.0;    // Shift everything to be < 1
      switch (value[index]) {
        case '9':
          fraction_result = fraction_result + 1.0;
        case '8':
          fraction_result = fraction_result + 1.0;
        case '7':
          fraction_result = fraction_result + 1.0;
        case '6':
          fraction_result = fraction_result + 1.0;
        case '5':
          fraction_result = fraction_result + 1.0;
        case '4':
          fraction_result = fraction_result + 1.0;
        case '3':
          fraction_result = fraction_result + 1.0;
        case '2':
          fraction_result = fraction_result + 1.0;
        case '1':
          fraction_result = fraction_result + 1.0;
        case '0':
          break;
        case '.':
          fraction_result = fraction_result * 10.0;    // Undo the magnitude shift
          index = 0;    // Break out of the for loop
          break;
        case 'e':
        case 'E':
          // Not yet handled
        default:
          break;
      }
    }
  }
  if (fractional) {
    result = result + fraction_result;
  }
  return result;
}

int skip_space(char* value) {
  // Skips whitespace and commas. Returns the number of
  // characters that were skipped.
  if (value == 0) {
    return 0;    // Null pointer
  }
  
  int offset = 0;
  while ((value[offset] == ' ') | (value[offset] == ',')) {
    offset++;
  }
  return offset;
}

void stripQuotes(char string[], char *buffer)
{
    
    int i;
    int stringLength;
    stringLength = strlen(string);
    
    for (i=1; i < stringLength; i++)
    {
      string[i-1] = string[i];
      
      
    }
    string[stringLength -2] = '\0';
    strcpy(string, buffer);

}


void run_command(char* name, char* value, currentObjectStructure *parsedObject) {
  // This is called for each "name":value pair found in the
  // incoming JSON. This is where you should put your handler
  // code.
  // There are a few important points to note:
  //  * This function, by default, will only be called for the
  //    top-level pairs, eg. given {"a":"b", "c":{"d":"e"}} it
  //    will be called with name="a", value="b" and name="c",
  //    value={"d":"e"}. It will not be called with name="d",
  //    value="e". If you want such recursion, add it yourself
  //    by calling read_commands on your JSON objects from
  //    somewhere within this function.
  //  * The name and value pointers will be free'd automatically
  //    after the JSON parser has finished. Thus, you should not
  //    store these pointers or any derived from them. If you
  //    want some data to persist, copy its values into some
  //    memory that you manage yourself.
  //  * Likewise, do not free these pointers yourself, as that
  //    will mangle the JSON reading.
  //  * The given pointers are not C-style strings (they are
  //    not terminated). Their length is implied by their JSON
  //    encoding. The variables "name_size" and "value_size"
  //    have been set up with the respective sizes for you if
  //    you need them.
  //  * The JSON formatting is still present in the pointers'
  //    values. For example, strings still contain their quotes.
  // Other than that, happy hacking!
  int name_size = value_length(name);
  int value_size = value_length(value);
  

  

  // We can't do simple string comparison since our pointers
 
  // are not null-terminated. Use compare_strings instead.
  if (compare_strings(name,"OBJECTID")) {
    strncpy( parsedObject->ObjectID, value, value_size);
    parsedObject->ObjectID[value_size] = '\0';
    stripQuotes(parsedObject->ObjectID, parsedObject->ObjectID);
   

  }
  if (compare_strings(name,"OBJECTSERVERID")) {

    strncpy( parsedObject->ObjectServerID, value, value_size); 
    parsedObject->ObjectServerID[value_size] = '\0';  
    stripQuotes(parsedObject->ObjectServerID, parsedObject->ObjectServerID);
 
  }
  if (compare_strings(name,"PASSWORD")) {
    strncpy( parsedObject->Password, value, value_size);  
    parsedObject->Password[value_size] = '\0';
    stripQuotes(parsedObject->Password, parsedObject->Password);
  
 
  }
    if (compare_strings(name,"OBJECTTYPE")) {
    strncpy( parsedObject->ObjectType, value, value_size);  
    parsedObject->ObjectType[value_size] = '\0';
    stripQuotes(parsedObject->ObjectType, parsedObject->ObjectType);
 
 
  }  if (compare_strings(name,"VALIDATE")) {
    strncpy( parsedObject->Validate, value, value_size);  
    parsedObject->Validate[value_size] = '\0';
    stripQuotes(parsedObject->Validate, parsedObject->Validate);
 
 
  }  
  if (compare_strings(name,"OBJECTFLAGS")) {
    strncpy( parsedObject->ObjectFlags, value, value_size);  
   parsedObject-> ObjectFlags[value_size] = '\0';
    stripQuotes(parsedObject->ObjectFlags, parsedObject->ObjectFlags);
 
 
  }  
  if (compare_strings(name,"OBJECTACTION")) {
    strncpy( parsedObject->ObjectAction, value, value_size);  
    parsedObject->ObjectAction[value_size] = '\0';
    stripQuotes(parsedObject->ObjectAction, parsedObject->ObjectAction);
 
 
  }
  if (compare_strings(name,"OBJECTNAME")) {
    strncpy( parsedObject->ObjectName, value, value_size);  
    parsedObject->ObjectName[value_size] = '\0';
    stripQuotes(parsedObject->ObjectName, parsedObject->ObjectName);  
 
 
  }

}




