--TEST--
Switch where all targets, including default, coincide
--EXTENSIONS--
opcache
--FILE--
<?php

$foo = 42.0;
$bar = true;

switch ($foo) {
default:
case 0: case 1: case 2: case 3:
    if ($bar) {
        echo "true\n";
    } else {
        echo "false\n";
    }
}

?>
--EXPECT--
true
