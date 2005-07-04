--TEST--
a script should not be able to modify session.use_trans_sid
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.use_trans_sid=1
session.use_cookies=0
session.cache_limiter=
register_globals=1
session.bug_compat_42=1
session.bug_compat_warn=0
session.name=PHPSESSID
session.serialize_handler=php
session.save_handler=files
--FILE--
<?php
error_reporting(E_ALL);

session_id("abtest");
session_start();

?>
<a href="/link">
<?php
ini_set("session.use_trans_sid","0");
?>
<a href="/link">
<?php
ini_set("session.use_trans_sid","1");
?>
<a href="/link">
<?php
session_destroy();
?>
--EXPECTF--
<a href="/link?PHPSESSID=abtest">

Warning: ini_set(): A session is active. You cannot change the session module's ini settings at this time. in %s on line %d
<a href="/link?PHPSESSID=abtest">

Warning: ini_set(): A session is active. You cannot change the session module's ini settings at this time. in %s on line %d
<a href="/link?PHPSESSID=abtest">
