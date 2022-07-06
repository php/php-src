--TEST--
Test dirname() function : regression with path normalization
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != "WIN")
  die("skip Only valid for Windows");
?>
--FILE--
<?php

$s = '/php_sanity/sanity.php?';
while (dirname($s) == "/php_sanity" && strlen($s) < 10000) {
    $s .= str_repeat('X', 250);
}

if (strlen($s) >= 10000) {
    echo "OK\n";
} else {
    print "ERROR: " . PHP_EOL;
    var_dump(dirname($s));
    var_dump(strlen($s));
}
?>
--EXPECT--
OK
