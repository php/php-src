--TEST--
Bug #73203 (passing additional_parameters causes mail to fail)
--DESCRIPTION--
We're not really interested in testing mail() here, but it is currently the
only function besides mb_send_mail() which allows to call php_escape_shell_cmd()
with an empty string. Therefore we don't check the resulting email, but only
verify that the call succeeds.
--INI--
sendmail_path=cat >/dev/null
mail.add_x_header = Off
--SKIPIF--
<?php 
if (substr(PHP_OS, 0, 3) === 'WIN') die('skip won\'t run on Windows');
?>
--FILE--
<?php
var_dump(
    mail('test@example.com', 'subject', 'message', 'From: lala@example.com', '')
);
?>
===DONE===
--EXPECT--
bool(true)
===DONE===
