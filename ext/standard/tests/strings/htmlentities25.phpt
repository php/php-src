--TEST--
htmlentities() should not be influenced by mb_internal_encoding()
--INI--
default_charset=
internal_encoding=
--EXTENSIONS--
mbstring
--FILE--
<?php

var_dump(htmlentities('äöü'));

?>
--EXPECT--
string(18) "&auml;&ouml;&uuml;"
