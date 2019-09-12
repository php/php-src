--TEST--
auto_detect_line_endings --INI-- bool
--INI--
auto_detect_line_endings=on
--STDIN--
fooBar1fooBar2fooBar3
--FILE--
<?php

var_dump(ini_get("auto_detect_line_endings"));

var_dump(fgets(STDIN));
var_dump(fgets(STDIN));
var_dump(fgets(STDIN));

echo "Done\n";
?>
--EXPECTF--
string(1) "1"
string(8) "fooBar1"
string(8) "fooBar2"
string(8) "fooBar3
"
Done
