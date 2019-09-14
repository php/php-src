--TEST--
ini_set auto_detect_line_endings bool
--FILE--
<?php

ini_set("auto_detect_line_endings", "on");
var_dump(ini_get("auto_detect_line_endings"));

$filePath = __DIR__ . DIRECTORY_SEPARATOR . "auto_detect_line_endings_2.txt";
file_put_contents($filePath, "fooBar1\rfooBar2\rfooBar3");

$stdin = fopen($filePath, "r");
var_dump(fgets($stdin));
var_dump(fgets($stdin));
var_dump(fgets($stdin));

echo "Done\n";
?>
--EXPECTF--
string(2) "on"
string(8) "fooBar1"
string(8) "fooBar2"
string(7) "fooBar3"
Done
--CLEAN--
<?php
unlink(__DIR__ . DIRECTORY_SEPARATOR . "auto_detect_line_endings_2.txt");
?>
