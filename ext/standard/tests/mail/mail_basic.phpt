--TEST--
Test mail() function : basic functionality
--INI--
sendmail_path={MAIL:mailBasic.out}
mail.add_x_header = Off
--FILE--
<?php
echo "*** Testing mail() : basic functionality ***\n";


// Initialise all required variables
$to = 'user@example.com';
$subject = 'Test Subject';
$message = 'A Message';
$additional_headers = 'KHeaders';
$outFile = "mailBasic.out";
@unlink($outFile);

echo "-- All Mail Content Parameters --\n";
// Calling mail() with all additional headers
var_dump( mail($to, $subject, $message, $additional_headers) );
echo file_get_contents($outFile);
unlink($outFile);

echo "\n-- Mandatory Parameters --\n";
// Calling mail() with mandatory arguments
var_dump( mail($to, $subject, $message) );
echo file_get_contents($outFile);
unlink($outFile);

?>
--EXPECT--
*** Testing mail() : basic functionality ***
-- All Mail Content Parameters --
bool(true)
To: user@example.com
Subject: Test Subject
KHeaders

A Message

-- Mandatory Parameters --
bool(true)
To: user@example.com
Subject: Test Subject

A Message
