--TEST--
Bug #69115 crash in mail (plus indirect pcre test)
--FILE--
<?php
/* Just ensure it doesn't crash when trimming headers */
$message = "Line 1\r\nLine 2\r\nLine 3";
mail('caffeinated@not-ever-reached-example.com', 'My Subject', $message, "From: me@me.me");
?>
===DONE===
--EXPECTF--
%A
===DONE===
