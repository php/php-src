--TEST--
Test mail() function : basic functionality 
--INI--
sendmail_path=echo --- > mailBasic2.out
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
$additional_headers = 'KHeaders';
$additional_parameters = "Extras";
$outFile = "mailBasic2.out";
@unlink($outFile);

echo "-- extra parameters --\n";
// Calling mail() with all possible arguments
var_dump( mail($to, $subject, $message, $additional_headers, $additional_parameters) );

//This test is just using a shell command (see the INI setting). The sleep()
//is used because that can take a while. If you see the test failing sometimes try 
//increasing the length of the sleep. 

sleep(5);
echo file_get_contents($outFile);
//unlink($outFile);
?>
===DONE===
--EXPECT--
*** Testing mail() : basic functionality ***
-- extra parameters --
bool(true)
--- Extras
===DONE===
