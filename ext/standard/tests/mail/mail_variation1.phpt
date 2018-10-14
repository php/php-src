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
/* Prototype  : int mail(string to, string subject, string message [, string additional_headers [, string additional_parameters]])
 * Description: Send an email message
 * Source code: ext/standard/mail.c
 * Alias to functions:
 */

echo "*** Testing mail() : variation ***\n";

// Initialise all required variables
$to = 'user@example.com';
$subject = 'Test Subject';
$message = 'A Message';
var_dump( mail($to, $subject, $message) );
?>
===DONE===
--EXPECT--
*** Testing mail() : variation ***
bool(false)
===DONE===
