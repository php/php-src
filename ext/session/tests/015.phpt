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
$sid = SID;
?>
<a href="/link?<?=$sid ?>">
<?php
session_destroy();
?>
--EXPECTF--
Deprecated: PHP Startup: Disabling session.use_only_cookies INI setting is deprecated in Unknown on line 0

Deprecated: PHP Startup: Enabling session.use_trans_sid INI setting is deprecated in Unknown on line 0

Deprecated: Constant SID is deprecated in %s on line 6
<a href="/link?PHPSESSID=test015&PHPSESSID=test015">
