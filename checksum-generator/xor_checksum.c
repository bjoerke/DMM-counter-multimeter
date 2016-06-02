#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char **argv)
{
   //reserve array
   int numValues = argc - 1;    //argv[0] is the program name
   uint8_t* values = (uint8_t*) malloc( (argc-1) * sizeof(uint8_t));

   //read in values from command line
   char* endptr;
   for(int i=0; i<numValues; i++)
   {
      long value = strtol(argv[i+1], &endptr,0);
      if(*endptr != '\0')
      {
          value = strtol(argv[i+1], &endptr,16);
          if(*endptr != '\0')
          {
             printf("\"%s\" is not a number\n", argv[i+1]);
             free(values);
             return -1;
          }
      }
      if(value > 255 || value < 0)
      {
         printf("\"%s\" = %d is not storable in a byte\n", argv[i+1], (int) value);
      }
      values[i] = (uint8_t) value;
   }

   //calculate checksum
   uint8_t checksum = 0;
   for(int i=0; i<numValues; i++)
   {
      checksum ^= values[i];
   }
   printf("Checksum of %d values: 0x%02X\n", numValues, (unsigned int) checksum);
   free(values);
   return 0;
}
