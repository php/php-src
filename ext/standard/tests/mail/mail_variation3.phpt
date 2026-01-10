--TEST--
Test mail() function : variation sendmail temp fail
--INI--
sendmail_path=exit 75
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == "WIN")
    die("skip Won't run on Windows");
?>
--FILE--
<?php
echo "*** Testing mail() : variation ***\n";

// Initialise all required variables
$to = 'user@example.com';
$subject = 'Test Subject';
$message = 'A Message';
var_dump(mail($to, $subject, $message));
?>
--EXPECT--
*** Testing mail() : variation ***
bool(true)
