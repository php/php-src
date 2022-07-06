--TEST--
Test quotemeta() function : basic functionality
--FILE--
<?php

echo "*** Testing quotemeta() : basic functionality ***\n";

var_dump(quotemeta("Hello how are you ?"));
var_dump(quotemeta("(100 + 50) * 10"));
var_dump(quotemeta("\+*?[^]($)"));
?>
--EXPECT--
*** Testing quotemeta() : basic functionality ***
string(20) "Hello how are you \?"
string(19) "\(100 \+ 50\) \* 10"
string(20) "\\\+\*\?\[\^\]\(\$\)"
