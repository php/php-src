--TEST--
$session_array = explode(";", session_encode()); should not segfault.
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.use_cookies=0
session.cache_limiter=
register_globals=0
session.bug_compat_42=1
session.bug_compat_warn=0
--FILE--
<?php
error_reporting(E_ALL & ~E_NOTICE);

$session_array = explode(";", @session_encode());
print "I live\n";
?>
--EXPECT--
I live
