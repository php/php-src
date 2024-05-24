--TEST--
Disable additional command line flags from being passed to sendmail through mail()
--INI--
sendmail_path=tee mail_blacklist.eml >/dev/null
mail.allow_additional_sendmail_flags = Off
--FILE--
<?php
var_dump(mail('user@example.com', 'Test Subject', 'A Message', 'KHeaders', '-X new'));
?>
--EXPECT--
bool(true)
