--TEST--
Test session_start() function : variation
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "*** Testing session_start() : variation ***\n";

$_SESSION['blah'] = 'foo';
var_dump($_SESSION);
session_start();
var_dump($_SESSION);

session_destroy();
echo "Done";
ob_end_flush();

?>
--EXPECT--
*** Testing session_start() : variation ***
array(1) {
  ["blah"]=>
  string(3) "foo"
}
array(0) {
}
Done
