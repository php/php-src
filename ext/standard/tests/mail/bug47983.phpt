--TEST--
Bug #47983 (mixed LF and CRLF line endings in mail())
--INI--
sendmail_path={MAIL:bug47983.out}
--FILE--
<?php
var_dump(mail('user@example.com', 'Test Subject', 'A Message', 'KHeaders'));
$mail = file_get_contents('bug47983.out');
var_dump(preg_match_all('/(?<!\r)\n/', $mail));
?>
--CLEAN--
<?php
unlink('bug47983.out');
?>
--EXPECT--
bool(true)
int(0)
