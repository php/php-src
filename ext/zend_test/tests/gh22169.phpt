--TEST--
GH-22169: Ensure escaped strings in stubs are valid
--EXTENSIONS--
zend_test
--FILE--
<?php

// Avoid funny control characters in output...
// "BEGIN "         424547494e20
// "\n\r\t\v\e\f"   0a0d090b1b0c
// "\\\$\""         5c2422
// "\101\x41\u{41}" 414141
// " END"           20454e44
var_dump(bin2hex(_ZendTestClass::$doubleQuoteEscaped));
var_dump(_ZendTestClass::$singleQuoteEscaped);
var_dump(_ZendTestClass::$escapeInterpolated);
?>
--EXPECT--
string(44) "424547494e200a0d090b1b0c5c242241414120454e44"
string(43) "BEGIN \n\r\t\v\e\f\\\\$\"\101\x41\u{41} END"
string(14) "begin $ \$ end"

