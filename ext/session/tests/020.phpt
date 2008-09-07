--TEST--
rewriter uses arg_seperator.output for modifying URLs
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.use_cookies=0
session.cache_limiter=
session.use_trans_sid=1
arg_separator.output="&amp;"
session.name=PHPSESSID
session.serialize_handler=php
session.save_handler=files
--FILE--
<?php

error_reporting(E_ALL);

session_id("abtest");
session_start();
?>
<a href="link.php?a=b">
<?php
session_destroy();
?>
--EXPECT--
<a href="link.php?a=b&amp;PHPSESSID=abtest">
