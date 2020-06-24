--TEST--
Test mail() function : basic functionality
--INI--
sendmail_path="exit 1"
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == "WIN")
  die("skip Won't run on Windows");
?>
--FILE--
<?php
echo "*** Testing mail() : basic functionality ***\n";


// Initialise all required variables
$to = 'user@example.com';
$subject = 'Test Subject';
$message = 'A Message';

echo "-- failure --\n";
var_dump( mail($to, $subject, $message) );
?>
--EXPECT--
*** Testing mail() : basic functionality ***
-- failure --
bool(false)
