--TEST--
const at file scope is inlined by the optimizer, define() is not
--EXTENSIONS--
opcache
--INI--
opcache.enable_cli=1
opcache.opt_debug_level=0x20000
--FILE--
<?php

const CONST_VAL = 1;
define('DEFINE_VAL', 2);

function use_const() {
    return CONST_VAL;
}

function use_define() {
    return DEFINE_VAL;
}

?>
--EXPECTF--
$_main:
     ; (lines=%d, args=%d, vars=%d, tmps=%d)
     ; (after optimizer)
     ; %s
0000 DECLARE_CONST string("CONST_VAL") int(1)
0001 DECLARE_CONST string("DEFINE_VAL") int(2)
0002 RETURN int(1)

use_const:
     ; (lines=%d, args=%d, vars=%d, tmps=%d)
     ; (after optimizer)
     ; %s
0000 RETURN int(1)

use_define:
     ; (lines=%d, args=%d, vars=%d, tmps=%d)
     ; (after optimizer)
     ; %s
0000 T%d = FETCH_CONSTANT string("DEFINE_VAL")
0001 RETURN %s
