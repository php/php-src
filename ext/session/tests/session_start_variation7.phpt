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

session_start();

$_SESSION['colour'] = 'green';
$_SESSION['animal'] = 'cat';
$_SESSION['person'] = 'julia';
$_SESSION['age'] = 6;

var_dump($_SESSION);
var_dump(session_destroy());
var_dump($_SESSION);
session_start();
var_dump($_SESSION);

session_destroy();
echo "Done";
ob_end_flush();
?>
--EXPECT--
*** Testing session_start() : variation ***
array(4) {
  ["colour"]=>
  string(5) "green"
  ["animal"]=>
  string(3) "cat"
  ["person"]=>
  string(5) "julia"
  ["age"]=>
  int(6)
}
bool(true)
array(4) {
  ["colour"]=>
  string(5) "green"
  ["animal"]=>
  string(3) "cat"
  ["person"]=>
  string(5) "julia"
  ["age"]=>
  int(6)
}
array(0) {
}
Done
