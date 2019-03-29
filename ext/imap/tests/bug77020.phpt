--TEST--
Bug #77020 (null pointer dereference in imap_mail)
--SKIPIF--
<?php
if (!extension_loaded('imap')) die('skip imap extension not available');
?>
--FILE--
<?php
imap_mail('1', 1, NULL);
?>
===DONE===
--EXPECTF--
Warning: imap_mail(): No message string in mail command in %s on line %d
%A
===DONE===
