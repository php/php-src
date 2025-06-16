--TEST--
GH-17503 (Undefined float conversion in mb_convert_variables)
--EXTENSIONS--
mbstring
--FILE--
<?php
$a = array_fill(0, 500, "<blah>");
var_dump(mb_convert_variables("ASCII", ["UTF-8", "UTF-16"], $a));
?>
--EXPECT--
string(5) "UTF-8"
