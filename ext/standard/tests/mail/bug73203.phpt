--TEST--
Bug #73203 (passing additional_parameters causes mail to fail)
--DESCRIPTION--
We're not really interested in testing mail() here, but it is currently the
only function besides mb_send_mail() which allows to call php_escape_shell_cmd()
with an empty string. Therefore we don't check the resulting email, but only
verify that the call succeeds.
--INI--
sendmail_path={MAIL:bug73203.eml}
mail.add_x_header = Off
--FILE--
<?php
var_dump(
    mail('test@example.com', 'subject', 'message', 'From: lala@example.com', '')
);
?>
--EXPECT--
bool(true)
--CLEAN--
<?php
unlink('bug73203.eml');
?>
