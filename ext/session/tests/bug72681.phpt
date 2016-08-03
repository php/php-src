--TEST--
Bug #72681: PHP Session Data Injection Vulnerability
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
ini_set('session.serialize_handler', 'php');
session_start();
$_SESSION['_SESSION'] = 'ryat|O:8:"stdClass":0:{}';
session_write_close();
session_start();
var_dump($_SESSION);
?>
--EXPECT--
array(0) {
}
