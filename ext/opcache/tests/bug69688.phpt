--TEST--
Bug #69688 (segfault with eval and opcache fast shutdown)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.fast_shutdown=1
--EXTENSIONS--
opcache
--FILE--
<?php
eval('function g() {} function g2() {} function g3() {}');

eval('class A{} class B{} class C{}');

?>
okey
--EXPECT--
okey
