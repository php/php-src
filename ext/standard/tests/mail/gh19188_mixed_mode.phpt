--TEST--
GH-19188: mail.cr_lf_mode=mixed
--ENV--
TEST_NAME={TEST_NAME}
--INI--
sendmail_path={MAIL:{TEST_NAME}.out}
mail.cr_lf_mode=mixed
--FILE--
<?php
var_dump(mail('user@example.com', 'Test Subject', 'A Message', 'X-Test: mixed'));
$mail = file_get_contents(getenv('TEST_NAME') . '.out');
echo "Mixed mode:\n";
var_dump(preg_match_all('/(?<!\r)\n/', $mail));
?>
--CLEAN--
<?php
@unlink(getenv('TEST_NAME') . '.out');
?>
--EXPECT--
bool(true)
Mixed mode:
int(5)
