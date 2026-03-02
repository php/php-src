--TEST--
Bug #72681: PHP Session Data Injection Vulnerability
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
ini_set('session.serialize_handler', 'php');
session_start();
$GLOBALS['ryat'] = $_SESSION;
$_SESSION['ryat'] = 'ryat|O:8:"stdClass":0:{}';
session_write_close();
session_start();
var_dump($ryat);
var_dump($_SESSION);
?>
--EXPECT--
array(0) {
}
array(1) {
  ["ryat"]=>
  string(24) "ryat|O:8:"stdClass":0:{}"
}
