#ifndef _PROTOCAL_H_
#define _PROTOCAL_H_

extern int SetupPack(uint8_t *pSourceData, uint16_t SrcLen, uint8_t *pDesData);
extern int Parse(uint8_t *pSourceData, uint16_t *Head, uint16_t *Tail, uint16_t buffer_size, uint8_t *pParseData);
extern int SetupPack2(uint8_t *pSourceData, uint16_t SrcLen, uint8_t *pDesData);
extern int Parse2(uint8_t *pSourceData, uint16_t *Head, uint16_t *Tail, uint16_t buffer_size, uint8_t *pParseData);


#endif
