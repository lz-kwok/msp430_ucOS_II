#ifndef FILETXT_H
#define FILETXT_H

extern char TimeString[20];



void WriteDataToSD(DataStruct *DataPointer);
void Create_filename();
void Write_dataToTxt(const char *file_path,char *dat);


#endif
