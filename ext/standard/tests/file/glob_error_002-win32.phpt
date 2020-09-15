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

try {
    glob(str_repeat('x', 3000));
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

echo "Done";
?>
--EXPECTF--
*** Testing glob() : error condition - pattern too long. ***
glob(): Argument #1 ($pattern) must have a length less than %d bytes
Done
