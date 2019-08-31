--TEST--
Bug #69874: Null addtional_headers does not send mail
--INI--
sendmail_path=tee mailBug69874.out >/dev/null
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

echo "*** Testing mail() : send email without additional headers ***\n";

// Initialise all required variables
$to = 'user@company.com';
$subject = 'Test Subject';
$message = 'A Message';

$outFile = "mailBug69874.out";
@unlink($outFile);

var_dump( mail($to, $subject, $message) );
echo file_get_contents($outFile);
unlink($outFile);

?>
===DONE===
--EXPECT--
*** Testing mail() : send email without additional headers ***
bool(true)
To: user@company.com
Subject: Test Subject

A Message
===DONE===
