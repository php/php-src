--TEST--
There should not be any warning with regard to redefining SID
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.use_cookies=0
session.cache_limiter=
register_globals=1
session.bug_compat_42=1
session.bug_compat_warn=0
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
