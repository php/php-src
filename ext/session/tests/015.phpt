--TEST--
use_trans_sid should not affect SID
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.use_trans_sid=1
session.use_cookies=0
session.cache_limiter=
arg_separator.output=&
session.name=PHPSESSID
--FILE--
<?php
error_reporting(E_ALL);

session_id("abtest");
session_start();
?>
<a href="/link?<?php echo SID; ?>">
<?php
session_destroy();
?>
--EXPECT--
<a href="/link?PHPSESSID=abtest&PHPSESSID=abtest">
