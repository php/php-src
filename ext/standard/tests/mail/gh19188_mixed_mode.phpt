--TEST--
GH-19188: mail.cr_lf_mode=mixed
--INI--
sendmail_path={MAIL:gh19188_mixed_mode.out}
mail.cr_lf_mode=mixed
--FILE--
<?php
var_dump(mail('user@example.com', 'Test Subject', 'A Message', 'X-Test: mixed'));
$mail = file_get_contents('gh19188_mixed_mode.out');
echo "Mixed mode:\n";
var_dump(preg_match_all('/(?<!\r)\n/', $mail));
?>
--CLEAN--
<?php
@unlink('gh19188_mixed_mode.out');
?>
--EXPECT--
bool(true)
Mixed mode:
int(5)
