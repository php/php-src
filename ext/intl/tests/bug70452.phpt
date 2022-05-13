--TEST--
Bug #70452 string IntlChar::charName() can sometimes return bool(false)
--EXTENSIONS--
intl
--FILE--
<?php
// Rely on the default value for the second parameter
var_dump(IntlChar::charName("A"));
// Provide a valid option for the second parameter
var_dump(IntlChar::charName("A", IntlChar::UNICODE_CHAR_NAME));
// Another valid option, but with no corresponding name for that given option
// This properly returns an empty string, as expected
var_dump(IntlChar::charName("A", IntlChar::UNICODE_10_CHAR_NAME));
// Provide an invalid value for the second parameter
var_dump(IntlChar::charName("A", 12345));
?>
--EXPECT--
string(22) "LATIN CAPITAL LETTER A"
string(22) "LATIN CAPITAL LETTER A"
string(0) ""
NULL
