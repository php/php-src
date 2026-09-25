--TEST--
session_cache_expire() overflow
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "*** Testing session_cache_expire() : overflow test ***\n";

session_cache_limiter("public");
var_dump(session_cache_expire((int)(PHP_INT_MAX/60)));
session_start();
var_dump(session_cache_expire() * 60);

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_cache_expire() : overflow test ***
int(180)
int(%s)
Done

