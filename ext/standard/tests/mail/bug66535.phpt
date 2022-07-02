--TEST--
Bug #66535: Extra newline if add_x_header and no additional headers are used
--INI--
sendmail_path={MAIL:mailBug66535.out}
mail.add_x_header = On
--FILE--
<?php
echo "*** Testing mail() : send email without additional headers ***\n";

// Initialise all required variables
$to = 'user@company.com';
$subject = 'Test Subject';
$message = 'A Message';

$outFile = "mailBug66535.out";
@unlink($outFile);

var_dump( mail($to, $subject, $message) );
echo file_get_contents($outFile);
unlink($outFile);

?>
--EXPECTF--
*** Testing mail() : send email without additional headers ***
bool(true)
To: user@company.com
Subject: Test Subject
X-PHP-Originating-Script: %d:bug66535.php

A Message
