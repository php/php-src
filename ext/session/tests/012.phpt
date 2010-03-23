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
--EXPECTF--
Warning: Directive 'register_globals' is deprecated in PHP 5.3 and greater in Unknown on line 0

Deprecated: Function session_register() is deprecated in %s on line %d
I live
