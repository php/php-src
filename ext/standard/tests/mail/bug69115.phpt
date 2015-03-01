--TEST--
Bug #69115 crash in mail (plus indirect pcre test)
--SKIPIF--
<?php
if( substr(PHP_OS, 0, 3) != 'WIN' ) {
   die('skip...Windows only test');
}

require_once(dirname(__FILE__).'/mail_skipif.inc');
?>
--FILE--
<?php
/* Just ensure it doesn't crash when trimming headers */
$message = "Line 1\r\nLine 2\r\nLine 3";
mail('user@example.com', 'My Subject', $message, "From: me@me.me");
?>
===DONE===
--EXPECT--
===DONE===
