--TEST--
GH-12481: PHP crash on Windows 64-bit with JIT enabled
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php
function foo(&$var) {
    $var &= 0xFFFFFFFF;
    return intval($var);
}
$v = 0x7FFFFFFF1;
for ($i = 0; $i < 10; $i++) {
    foo($v);
}
?>
DONE
--EXPECTF--
DONE
