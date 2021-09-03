--TEST--
Test gzencode() function : variation
--EXTENSIONS--
zlib
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != "WIN") {
  die("skip only for Windows");
}
?>
--FILE--
<?php

include(__DIR__ . '/data.inc');

echo "*** Testing gzencode() : variation ***\n";

echo "\n-- Testing multiple compression --\n";
$output = gzencode(gzencode($data));

$back = gzdecode(gzdecode($output));
var_dump($data === $back);
?>
--EXPECT--
*** Testing gzencode() : variation ***

-- Testing multiple compression --
bool(true)
