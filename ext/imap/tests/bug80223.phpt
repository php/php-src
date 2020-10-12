--TEST--
Bug #80223 (imap_mail_compose() leaks envelope on malformed bodies)
--SKIPIF--
<?php
if (!extension_loaded('imap')) die('skip imap extension not available');
?>
--FILE--
<?php
imap_mail_compose([], []);
imap_mail_compose([], [1]);
?>
--EXPECTF--
Warning: imap_mail_compose(): body parameter must be a non-empty array in %s on line %d

Warning: imap_mail_compose(): body parameter must be a non-empty array in %s on line %d
