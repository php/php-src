--TEST--
registering $_SESSION should not segfault
--EXTENSIONS--
session
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

### Absurd example, value of $_SESSION does not matter

session_id("test012");
session_start();
$_SESSION["_SESSION"] = Array();
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
