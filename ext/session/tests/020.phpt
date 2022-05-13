--TEST--
rewriter uses arg_separator.output for modifying URLs
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.use_cookies=0
session.use_only_cookies=0
session.use_strict_mode=0
session.cache_limiter=
session.use_trans_sid=1
arg_separator.output="&amp;"
session.name=PHPSESSID
session.serialize_handler=php
session.save_handler=files
--FILE--
<?php

error_reporting(E_ALL);

session_id("test020");
session_start();
?>
<a href="link.php?a=b">
<?php
session_destroy();
?>
--EXPECT--
<a href="link.php?a=b&amp;PHPSESSID=test020">
