--TEST--
a script should not be able to modify session.use_trans_sid
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.use_trans_sid=0
session.use_cookies=0
session.use_strict_mode=0
session.cache_limiter=
session.name=PHPSESSID
session.serialize_handler=php
session.save_handler=files
--FILE--
<?php
error_reporting(E_ALL);

session_id("test014");
session_start();

?>
<a href="/link">
<?php
ini_set("session.use_trans_sid","1");
?>
<a href="/link">
<?php
ini_set("session.use_trans_sid","0");
?>
<a href="/link">
<?php
session_destroy();
?>
--EXPECTF--
<a href="/link">

Warning: ini_set(): Session ini settings cannot be changed when a session is active in %s on line %d
<a href="/link">

Warning: ini_set(): Session ini settings cannot be changed when a session is active in %s on line %d
<a href="/link">
