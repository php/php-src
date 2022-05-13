--TEST--
session_decode(); should not segfault
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.use_cookies=0
session.cache_limiter=
--FILE--
<?php
error_reporting(E_ALL);

session_decode("garbage data and no session started");
session_decode("userid|s:5:\"mazen\";chatRoom|s:1:\"1\";");
print "I live\n";
?>
--EXPECTF--
Warning: session_decode(): Session data cannot be decoded when there is no active session in %s on line %d

Warning: session_decode(): Session data cannot be decoded when there is no active session in %s on line %d
I live
