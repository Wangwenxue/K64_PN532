
#ifndef __PN532_H__
#define __PN532_H__


void CleanBuffer(uint16_t num);
void nfc_WakeUp(void);
void  nfc_InListPassiveTarget(void);
void  nfc_PsdVerifyKeyA(void);
void  nfc_write(uint8_t buf[16]);
void  nfc_read();

#endif /* __PN532_H__ */