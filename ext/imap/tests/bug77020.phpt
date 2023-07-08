--TEST--
Bug #77020 (null pointer dereference in imap_mail)
--EXTENSIONS--
imap
--INI--
sendmail_path="echo >/dev/null"
--FILE--
<?php
// For Windows, set it to a string of length HOST_NAME_LEN (256) so the mail is not actually sent
ini_set("SMTP", str_repeat("A", 256));

@imap_mail('1', 1, NULL);
echo 'done'
?>
--EXPECTF--
%Adone
