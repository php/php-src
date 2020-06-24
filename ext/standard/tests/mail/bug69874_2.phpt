--TEST--
Bug #69874: Null addtional_headers does not send mail
--INI--
sendmail_path={MAIL:mailBug69874_2.out}
mail.add_x_header = On
--FILE--
<?php
echo "*** Testing mail() : send email without additional headers ***\n";

// Initialise all required variables
$to = 'user@company.com';
$subject = 'Test Subject';
$message = 'A Message';

$outFile = "mailBug69874_2.out";
@unlink($outFile);

var_dump( mail($to, $subject, $message, '') );
echo file_get_contents($outFile);
unlink($outFile);

?>
--EXPECTF--
*** Testing mail() : send email without additional headers ***
bool(true)
To: user@company.com
Subject: Test Subject
X-PHP-Originating-Script: %d:bug69874_2.php

A Message
