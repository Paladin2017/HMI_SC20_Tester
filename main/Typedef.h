#ifndef _TYPEDEFINE_H_
#define _TYPEDEFINE_H_

typedef struct 
{
  uint8_t EventID;
  uint8_t EventOpcode;
  void(*pProcessFun)(void* para);
  void(*pReackFun)(void* para);
}strHMICommand;

#endif