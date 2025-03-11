--TEST--
String interning during constants substitution
--INI--
opcache.enable_cli=0
--FILE--
<?php
define ("A", "." . ord(26) . ".");
eval("class A {const a = A;}");
var_dump(A::a);
?>
--EXPECT--
string(4) ".50."
