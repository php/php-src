--TEST--
Bug #71974 Trans sid will always be send, even if cookies are available
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.save_handler=files
session.auto_start=0
session.use_cookies=1
session.use_only_cookies=0
session.use_trans_sid=1
session.use_strict_mode=0
--COOKIE--
PHPSESSID=1234567890123456789012345678901234567890
--FILE--
<?php
ob_start();
session_start()
?>
<a href="some.php">abc</a>
--EXPECT--
<a href="some.php">abc</a>

Deprecated: PHP Startup: Disabling session.use_only_cookies INI setting is deprecated in Unknown on line 0

Deprecated: PHP Startup: Enabling session.use_trans_sid INI setting is deprecated in Unknown on line 0
