--TEST--
Bug #51604 (newline in end of header is shown in start of message)
--INI--
sendmail_path=tee mail_bug51604.out >/dev/null
mail.add_x_header = Off
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == "WIN")
  die("skip Won't run on Windows");
?>
--FILE--
<?php
// Initialise all required variables
$to = 'user@company.com';
$subject = 'Test Subject';
$message = 'A Message';
$additional_headers = "KHeaders\n\n\n\n\n";
$outFile = "mail_bug51604.out";
@unlink($outFile);

// Calling mail() with all additional headers
var_dump( mail($to, $subject, $message, $additional_headers) );
echo file_get_contents($outFile);
unlink($outFile);

?>
===DONE===
--EXPECT--
bool(true)
To: user@company.com
Subject: Test Subject
KHeaders

A Message
===DONE===
