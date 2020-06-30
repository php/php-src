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
   | Authors: David Korczynski <david@adalogics.com>                      |
   +----------------------------------------------------------------------+
 */
#include "fuzzer.h"

#include "Zend/zend.h"
#include "main/php_config.h"
#include "main/php_main.h"
#include "oniguruma.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "fuzzer-sapi.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
        if (Size < 200)
        {
            return 0;
        }

        char *args[2];
        char *data = malloc(101);
        memcpy(data, Data, 100);
        data[100] = '\0';

        char *data2 = malloc(Size - 99);
        Data += 100;
        memcpy(data2, Data, Size-100);
        data2[Size-100] = '\0';

        if (fuzzer_request_startup() == FAILURE) {
                return 0;
        }

        args[0] = data;
        args[1] = data2;
        fuzzer_call_php_func("preg_match", 2, args);

        php_request_shutdown(NULL);

        free(data);
        free(data2);
        return 0;
}

int LLVMFuzzerInitialize(int *argc, char ***argv) {
        fuzzer_init_php();
        return 0;
}

