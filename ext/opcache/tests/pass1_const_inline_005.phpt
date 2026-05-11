--TEST--
const declared only in a separate required file is not inlined by the optimizer
--EXTENSIONS--
opcache
--INI--
opcache.enable_cli=1
opcache.opt_debug_level=0x20000
--FILE--
<?php

require __DIR__ . '/pass1_const_inline_005.inc';

function use_external_const() {
    return EXTERNAL_CONST;
}

?>
--EXPECTF--
$_main:
     ; (lines=%d, args=%d, vars=%d, tmps=%d)
     ; (after optimizer)
     ; %s
0000 INCLUDE_OR_EVAL (require) string("%s")
0001 RETURN int(1)

use_external_const:
     ; (lines=%d, args=%d, vars=%d, tmps=%d)
     ; (after optimizer)
     ; %s
0000 T%d = FETCH_CONSTANT string("EXTERNAL_CONST")
0001 RETURN %s

$_main:
     ; (lines=%d, args=%d, vars=%d, tmps=%d)
     ; (after optimizer)
     ; %s
0000 DECLARE_CONST string("EXTERNAL_CONST") int(42)
0001 RETURN int(1)
