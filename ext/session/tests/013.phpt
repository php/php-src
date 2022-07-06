--TEST--
redefining SID should not cause warnings
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.use_cookies=0
session.use_strict_mode=0
session.cache_limiter=
session.serialize_handler=php
session.save_handler=files
--FILE--
<?php
error_reporting(E_ALL);

session_id("abtest");
session_start();
session_destroy();
session_id("abtest2");
session_start();
session_destroy();

print "I live\n";
?>
--EXPECT--
I live
