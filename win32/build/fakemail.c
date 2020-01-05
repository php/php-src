/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Christoph M. Becker <cmb@php.net>                            |
  +----------------------------------------------------------------------+
*/

/* This program can be used as sendmail replacement to write the email contents
   to a file, which is mainly useful for email related tests on Windows. 
   Usage: fakemail <path> */

#include <stdio.h>

int main(int argc, char *argv[])
{
    FILE *out;
    char c;

    if (argc != 2) {
        return 1;
    }
    if (!(out = fopen(argv[1], "w"))) {
        return 1;
    }
    while ((c = getchar()) != EOF) {
        putc(c, out);
    }
    fclose(out);

    return 0;
}
