--TEST--
Test gzencode() function : variation
--EXTENSIONS--
zlib
--SKIPIF--
<?php

if( substr(PHP_OS, 0, 3) == "WIN" ) {
   die("skip.. Do not run on Windows");
}



if (PHP_OS == "Darwin") {
    print "skip - OS is encoded in headers, tested header is non Darwin";
}
?>
--FILE--
<?php
include(__DIR__ . '/data.inc');

echo "*** Testing gzencode() : variation ***\n";

echo "\n-- Testing multiple compression --\n";
$output = gzencode($data);
var_dump(bin2hex(gzencode($output)));

?>
--EXPECTF--
*** Testing gzencode() : variation ***

-- Testing multiple compression --
string(36%d) "%s"
