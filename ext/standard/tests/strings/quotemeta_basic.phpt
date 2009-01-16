--TEST--
Test quotemeta() function : basic functionality 
--FILE--
<?php

/* Prototype  : string quotemeta  ( string $str  )
 * Description: Quote meta characters
 * Source code: ext/standard/string.c
*/

echo "*** Testing quotemeta() : basic functionality ***\n";

var_dump(quotemeta("Hello how are you ?"));
var_dump(quotemeta("(100 + 50) * 10"));
var_dump(quotemeta("\+*?[^]($)"));
?>
===DONE===
--EXPECTF--
*** Testing quotemeta() : basic functionality ***
unicode(20) "Hello how are you \?"
unicode(19) "\(100 \+ 50\) \* 10"
unicode(20) "\\\+\*\?\[\^\]\(\$\)"
===DONE===