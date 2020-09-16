--TEST--
Test glob() function: error condition - pattern too long.
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != "WIN")
  die("skip Only valid for Windows");
?>
--FILE--
<?php
echo "*** Testing glob() : error condition - pattern too long. ***\n";

glob(str_repeat('x', 3000));

echo "Done";
?>
--EXPECTF--
*** Testing glob() : error condition - pattern too long. ***

Warning: glob(): Pattern exceeds the maximum allowed length of %d characters in %s on line %d
Done
