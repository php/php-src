--TEST--
Test mail() function : variation sigterm
--INI--
sendmail_path="kill \$\$"
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
--EXPECTF--
*** Testing mail() : variation ***

Warning: mail(): Sendmail killed by signal %d (%s) in %smail_variation4.php on line %d
bool(false)
