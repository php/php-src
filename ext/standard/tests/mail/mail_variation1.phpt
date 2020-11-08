--TEST--
Test mail() function : variation invalid program for sendmail
--INI--
sendmail_path=rubbish 2>/dev/null
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == "WIN")
  die("skip Won't run on Windows");
?>
--FILE--
<?php
echo "*** Testing mail() : variation ***\n";

// Initialise all required variables
$to = 'user@example.com';
$subject = 'Test Subject';
$message = 'A Message';
var_dump( mail($to, $subject, $message) );
?>
--EXPECT--
*** Testing mail() : variation ***
bool(false)
