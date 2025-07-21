/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Samuel Melrose <sam@melroseandco.uk>                        |
   +----------------------------------------------------------------------+
*/

/*
 * Opcache ABI Signature Generator
 *
 * This program is compiled and run during the PHP build process.
 * It reads pre-processed C header source from stdin, calculates a
 * CRC32 checksum of it, and prints the hex digest to stdout.
 *
 * A simple CRC32 is used as a lightweight, dependency-free way to
 * detect changes in the core data structures.
 */

 #include <stdio.h>
 #include <stdint.h>
 
 /* A self-contained CRC32 implementation */
 static uint32_t crc32_table[256];
 
 static void build_crc32_table(void) {
     for (uint32_t i = 0; i < 256; i++) {
         uint32_t ch = i;
         uint32_t crc = 0;
         for (size_t j = 0; j < 8; j++) {
             uint32_t b = (ch ^ crc) & 1;
             crc >>= 1;
             if (b) {
                 crc = crc ^ 0xEDB88320;
             }
             ch >>= 1;
         }
         crc32_table[i] = crc;
     }
 }
 
 static uint32_t crc32_update(uint32_t crc, const unsigned char *buf, size_t len) {
     crc = ~crc;
     while (len--) {
         crc = (crc >> 8) ^ crc32_table[(crc ^ *buf++) & 0xFF];
     }
     return ~crc;
 }
 
 int main(int argc, char *argv[]) {
     uint32_t crc = 0;
     unsigned char buf[4096];
     size_t n;
 
     build_crc32_table();
 
     while ((n = fread(buf, 1, sizeof(buf), stdin)) > 0) {
         crc = crc32_update(crc, buf, n);
     }
 
     printf("%08x", crc);
 
     return 0;
 }
