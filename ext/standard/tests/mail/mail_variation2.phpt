--TEST--
Test mail() function : variation force extra parameters
--INI--
sendmail_path="sed > /tmp/php_test_mailVariation2.out"
mail.force_extra_parameters="-e4a---forced-params"
mail.add_x_header = Off
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

echo "*** Testing mail() : basic functionality ***\n";


// Initialise all required variables
$to = 'user@company.com';
$subject = 'Test Subject';
$message = 'A Message';
$outFile = "/tmp/php_test_mailVariation2.out";
@unlink($outFile);

var_dump( mail($to, $subject, $message) );
echo file_get_contents($outFile);
unlink($outFile);

?>
===DONE===
--EXPECT--
*** Testing mail() : basic functionality ***
bool(true)
To: user@company.com
Subject: Test Subject

A Message
---forced-params
===DONE===
