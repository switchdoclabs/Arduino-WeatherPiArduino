/* FRAM log Information */

// Arduino Logging into FRAM

// define logging constants
// entryData0

#define FRAMWeatherEntryType 0

#define FRAMEmptyEntry 0
#define FRAMUsedEntry 1


#define NOTREAD 0
#define SUCCESSFULREAD 1
#define TRIEDTOREAD 2
int FramEntryNextItem;
int FramLineSize;


void initializeFRAMTable(int dataSize)
{
  int lineSize;
  
  if (dataSize < 5)   //minimum size is 5
    dataSize = 5;
    
  lineSize = 3 + dataSize + 1;  // status bytes + entrylength + nullbyte
  int i;
  FramLineSize = lineSize;
  int tableSize;
 

  
  tableSize = 32768 / lineSize -1;
  Serial.print("FRAM Table Size =");  
  Serial.println(tableSize);
  
  for (i=0; i < tableSize; i++)
  {
     
    fram.write8(i*lineSize, 1);  // 1 = empty, 2 = full
    fram.write8(i*lineSize+1, NOTREAD); // status byte // 0 = not read, 1 = read successfull, -1 = tried to read
    fram.write8(i*lineSize+2, FRAMWeatherEntryType); // Entry type
    fram.write8(i*lineSize+3, 'E'); // empty string 
    fram.write8(i*lineSize+4, 'M'); // empty string
    fram.write8(i*lineSize+5, 'P'); // empty string
    fram.write8(i*lineSize+6, 'T'); // empty string
    fram.write8(i*lineSize+7, 'Y'); // empty string
    fram.write8(i*lineSize+8, '\0'); // empty string
   
    
        
    
  }
  
  FramEntryNextItem = 0;



  
}

time_t readRTCreliably();


void writeFramEntry(int type,  char newEntry[])
{
  
    int lineSize;
    lineSize = FramLineSize;

    fram.write8(FramEntryNextItem*lineSize, 2);  // 1 = empty, 2 = full
    fram.write8(FramEntryNextItem*lineSize+1, NOTREAD); // status byte // 0 = not read, 1 = read successfull, -1 = tried to read
    fram.write8(FramEntryNextItem*lineSize+2, FRAMWeatherEntryType); // Entry type
    int i;
    
    for (i = 0; i < strlen(newEntry); i++)
    {
      fram.write8(FramEntryNextItem*lineSize+3+i, newEntry[i]); // empty string

    }

    fram.write8(FramEntryNextItem*lineSize+3+i, '\0'); // empty string
    

   int tableSize;
 

  
    tableSize = 32768 / lineSize -1;
    FramEntryNextItem++;
    if (FramEntryNextItem >= tableSize)
      FramEntryNextItem = 0;

}



int returnFramUnreadCount()
{

  int i;
  int count;
  count = 0;
  
     int tableSize;
 

  
    tableSize = 32768 / FramLineSize -1;
  
  int statusByte;
  int EorF;
  for (i=0; i < tableSize; i++)
  {
    statusByte = fram.read8(FramLineSize*i+1);
    EorF = fram.read8(FramLineSize*i);

    if ((EorF== 2) && ((statusByte == NOTREAD) || (statusByte == TRIEDTOREAD)))
    {
      count++;
   

      
    }

  }
  return count;   // no unread elements
  

  
  
}

int FramfetchNextUnreadEntry()
{

  int i;
  int tableSize;
 

  
  tableSize = 32768 / FramLineSize -1;
  
  int statusByte;
  int EorF;
  for (i=0; i < tableSize; i++)
  {
    statusByte = fram.read8(FramLineSize*i+1);
    EorF = fram.read8(FramLineSize*i);
    
    if ((EorF ==2) && (statusByte == NOTREAD))
    {
      fram.write8(i*FramLineSize+1, TRIEDTOREAD);
      return i;
      
    }

  }
  return -1;   // no unread elements

  
}
//void returnJSONInValidResponse(char returnJSON[], char *buffer, currentObjectStructure *parsedObject);

void readFramItem (char returnItem[], char *buffer, int index, int markRead)
{

    if (fram.read8(FramLineSize*index) == 2)
    {
      if (markRead == 1)
      {
        fram.write8(FramLineSize*index, 1);
      }
      
      
      int dataSize = FramLineSize - 4;
          
      int j;
      for (j= 0; j < dataSize+1; j++)
      {
           returnItem[j] = fram.read8(index*FramLineSize+3+j);

           
      }
      
     
   
    
      strcpy(returnItem, buffer);
      return;
    
    }
    returnItem[0] = '\0';

   strcpy(returnItem, buffer);
}
/*

void convertLogTriedToReadToNotRead()
{
  
   int i;
  
  for (i=0; i < LOGENTRYTABLESIZE; i++)
  {
    
    if ((LogEntryArray[i].entryTime > 0) &&  (LogEntryArray[i].entryRead == TRIEDTOREAD))
    {
      LogEntryArray[i].entryRead = NOTREAD;
      
      
    }

  }
  
  
  
}

void convertLogTriedToReadToSuccess()
{
  
   int i;
  
  for (i=0; i < LOGENTRYTABLESIZE; i++)
  {
    
    if ((LogEntryArray[i].entryTime > 0) &&  (LogEntryArray[i].entryRead == TRIEDTOREAD))
    {
      LogEntryArray[i].entryRead = SUCCESSFULREAD;
      
      
    }

  }
  
  
  
}

*/
void displayFram()
{
  
    int lineSize;
 
    lineSize = FramLineSize;
   
    int tableSize;
 

  
    tableSize = 32768 / lineSize -1;
    
    int dataSize = FramLineSize - 4;
    int i;
    for (i=0; i < tableSize; i++)
    {
      char buffer2[150];
      buffer2[0] = 0;
      int EorF;
      EorF = fram.read8(i*lineSize);
      if (EorF == 2)
      {
         Serial.print("FramEntry: ");
         Serial.print(i);
         Serial.print(":");


         Serial.print(fram.read8(i*lineSize+0));
         Serial.print(" :");
         Serial.print(fram.read8(i*lineSize+1));
         Serial.print(":");
         Serial.print(fram.read8(i*lineSize+2));
         Serial.print(":");
         
         int j;
         for (j= 0; j < dataSize+1; j++)
         {
           buffer2[j] = fram.read8(i*lineSize+3+j);

           
         }
         Serial.print(buffer2);
         Serial.print(":");

         Serial.println();
      }
      
    }
  
}
