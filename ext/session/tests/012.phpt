--TEST--
registering $_SESSION should not segfault
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.use_cookies=0
session.cache_limiter=
register_globals=1
session.bug_compat_42=1
session.bug_compat_warn=0
session.serialize_handler=php
session.save_handler=files
--FILE--
<?php
error_reporting(E_ALL);

### Absurd example, value of $_SESSION does not matter

session_id("abtest");
session_start();
session_register("_SESSION");
$_SESSION = "kk";

session_write_close();

### Restart to test for $_SESSION brokenness

session_start();
$_SESSION = "kk";
session_destroy();

print "I live\n";
?>
--EXPECT--
I live
