--TEST--
Test session_start() function : variation
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/* 
 * Prototype : bool session_start(void)
 * Description : Initialize session data
 * Source code : ext/session/session.c 
 */

echo "*** Testing session_start() : variation ***\n";

$_SESSION['blah'] = 'foo';
var_dump($_SESSION);
session_start();
var_dump($_SESSION);

session_destroy();
echo "Done";
ob_end_flush();

?>
--EXPECTF--
*** Testing session_start() : variation ***
array(1) {
  ["blah"]=>
  string(3) "foo"
}
array(0) {
}
Done

