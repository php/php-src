--TEST--
htmlentities() test 11 (default_charset / ISO-8859-15)
--INI--
output_handler=
default_charset=ISO-8859-15
--FILE--
<?php
print ini_get('default_charset')."\n";
var_dump(htmlentities("\xbc\xbd\xbe", ENT_QUOTES, ''));
?>
--EXPECT--
ISO-8859-15
string(20) "&OElig;&oelig;&Yuml;"
