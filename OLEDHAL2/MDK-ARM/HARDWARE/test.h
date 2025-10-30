#ifndef __TEST_H
#define __TEST_H
 
#include "main.h"
#include "u8g2.h"
 
void testDrawProcess(u8g2_t *u8g2);
void testDrawInfo(u8g2_t *u8g2);
void testSlideDisplay(u8g2_t *u8g2, uint8_t slide_type);
void testDrawCustomPattern(u8g2_t *u8g2);
void testShowFont(u8g2_t *u8g2);
void testDrawFrame(u8g2_t *u8g2);
void testDrawRBox(u8g2_t *u8g2);
void testDrawCircle(u8g2_t *u8g2);
void testDrawFilledEllipse(u8g2_t *u8g2);
void testDrawMulti(u8g2_t *u8g2);
void testDrawXBM(u8g2_t *u8g2);
void U8g2_ShowCustomImage(u8g2_t *u8g2, uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint8_t *pic, uint8_t mode);
void u8g2DrawTest(u8g2_t *u8g2);

#endif
