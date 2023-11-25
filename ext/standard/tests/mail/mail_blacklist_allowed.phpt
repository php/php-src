--TEST--
Enable additional command line flags from being passed to sendmail through mail()
--INI--
sendmail_path=tee mail_blacklist.eml >/dev/null
mail.allow_additional_sendmail_flags = On
--FILE--
<?php
var_dump(mail('user@example.com', 'Test Subject', 'A Message', 'KHeaders', '-X new'));
?>
--EXPECT--
tee: invalid option -- 'X'
Try 'tee --help' for more information.
bool(false)
