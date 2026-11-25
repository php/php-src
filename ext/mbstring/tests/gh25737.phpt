--TEST--
GH-25737 (mb_send_mail() ignores $additional_params)
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (PHP_OS_FAMILY === "Windows") die("skip Won't run on Windows");
?>
--INI--
sendmail_path="cat > /dev/null; echo sendmail arguments:"
--FILE--
<?php
var_dump(mb_send_mail("user@example.com", "subject", "message", [], "-fsender@example.com"));
?>
--EXPECT--
sendmail arguments: -fsender@example.com
bool(true)
