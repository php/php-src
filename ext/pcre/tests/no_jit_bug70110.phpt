--TEST--
Test (*NO_JIT) doesn't crash when JIT enabled
--SKIPIF--
<?php if (!PCRE_JIT_SUPPORT) die("skip pcre jit support required"); ?>
--FILE--
<?php

var_dump(preg_match('/(*NO_JIT)^(A{1,2}B)+$$/',str_repeat('AB',8192)));
var_dump(preg_match('~(*NO_JIT)(a)*~', str_repeat('a', 5431), $match))

?>
==DONE==
--EXPECT--
int(1)
int(1)
==DONE==
