--TEST--
GH-20257: heap overflow with empty message and mail.cr_lf_mode=lf set
--INI--
sendmail_path="exit 0"
mail.cr_lf_mode=lf
--CREDITS--
YuanchengJiang
--FILE--
<?php
$to = "user@example.com";
$subject = $message = "";
var_dump(mail($to, $subject, $message));
?>
--EXPECT--
bool(true)
