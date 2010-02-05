--TEST--
Test mail() function : basic functionality 
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == "WIN")
  die("skip Won't run on Windows");
?>
--INI--
sendmail_path="sed > /tmp/php_test_mailBasic2.out"
mail.add_x_header = Off
--FILE--
<?php
/* Prototype  : int mail(string to, string subject, string message [, string additional_headers [, string additional_parameters]])
 * Description: Send an email message 
 * Source code: ext/standard/mail.c
 * Alias to functions: 
 */

echo "*** Testing mail() : basic functionality ***\n";


// Initialise all required variables
$to = 'user@company.com';
$subject = 'Test Subject';
$message = 'A Message';
$additional_headers = 'KHeaders';
$additional_parameters = "-e '5 a--- Extras'";
$outFile = "/tmp/php_test_mailBasic2.out";
@unlink($outFile);

echo "-- extra parameters --\n";
// Calling mail() with all possible arguments
var_dump( mail($to, $subject, $message, $additional_headers, $additional_parameters) );

echo file_get_contents($outFile);
unlink($outFile);
?>
===DONE===
--EXPECT--
*** Testing mail() : basic functionality ***
-- extra parameters --
bool(true)
To: user@company.com
Subject: Test Subject
KHeaders

A Message
--- Extras
===DONE===
