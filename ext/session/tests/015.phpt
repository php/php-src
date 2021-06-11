--TEST--
use_trans_sid should not affect SID
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.use_trans_sid=1
session.use_cookies=0
session.use_only_cookies=0
session.use_strict_mode=0
session.cache_limiter=
arg_separator.output=&
session.name=PHPSESSID
session.serialize_handler=php
session.save_handler=files
--FILE--
<?php
error_reporting(E_ALL);

session_id("test015");
session_start();
?>
<a href="/link?<?php echo SID; ?>">
<?php
session_destroy();
?>
--EXPECT--
<a href="/link?PHPSESSID=test015&PHPSESSID=test015">
