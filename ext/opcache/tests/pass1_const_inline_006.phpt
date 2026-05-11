--TEST--
const redeclared after being declared in a separate required file: optimizer inlines local value, runtime warning
--EXTENSIONS--
opcache
--INI--
opcache.enable_cli=1
opcache.opt_debug_level=0x20000
--FILE--
<?php

require __DIR__ . '/pass1_const_inline_006.inc';

const SHARED_CONST = 99;

function use_shared_const() {
    return SHARED_CONST;
}

var_dump(use_shared_const());

?>
--EXPECTF--
$_main:
     ; (lines=%d, args=%d, vars=%d, tmps=%d)
     ; (after optimizer)
     ; %s
0000 INCLUDE_OR_EVAL (require) string("%s")
0001 DECLARE_CONST string("SHARED_CONST") int(99)
0002 INIT_FCALL 1 %d string("var_dump")
0003 INIT_FCALL 0 %d string("use_shared_const")
0004 T%d = DO_UCALL
0005 SEND_VAL T%d 1
0006 DO_ICALL
0007 RETURN int(1)

use_shared_const:
     ; (lines=%d, args=%d, vars=%d, tmps=%d)
     ; (after optimizer)
     ; %s
0000 RETURN int(99)

$_main:
     ; (lines=%d, args=%d, vars=%d, tmps=%d)
     ; (after optimizer)
     ; %s
0000 DECLARE_CONST string("SHARED_CONST") int(42)
0001 RETURN int(1)

Warning: Constant SHARED_CONST already defined, this will be an error in PHP 9 in %s on line %d
int(99)
