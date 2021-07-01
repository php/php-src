--TEST--
Bug #77020 (null pointer dereference in imap_mail)
--EXTENSIONS--
imap
--FILE--
<?php
@imap_mail('1', 1, NULL);
echo 'done'
?>
--EXPECTF--
%Adone
