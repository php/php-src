--TEST--
Bug #51604 (newline in end of header is shown in start of message)
--INI--
sendmail_path={MAIL:mail_bug51604.out}
mail.add_x_header = Off
--FILE--
<?php
// Initialise all required variables
$to = 'user@example.com';
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
--EXPECT--
bool(true)
To: user@example.com
Subject: Test Subject
KHeaders

A Message
