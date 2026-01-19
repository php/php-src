--TEST--
GH-20858 Null pointer dereference in php_mail_detect_multiple_crlf via error_log
--INI--
sendmail_path={MAIL:{PWD}/gh20858.eml}
mail.add_x_header=off
--FILE--
<?php

$headers  = "From: test <mail@domain.tld>\n";
$headers .= "Cc: test <mail@domain.tld>\n"; 
$headers .= 'X-Mailer: PHP/' . phpversion();

// Send mail with nothing set
var_dump(error_log("Error message", 1, null));

// Send mail with destination set
var_dump(error_log("Error message with dest", 1, "default@domain.tld", null));

// Send mail with custom headers and no mailer to
var_dump(error_log("Error message cust headers", 1, null, $headers));

// Send mail with destination set + custom headers
var_dump(error_log("Error message with both", 1, "default@domain.tld", $headers));
?>
--CLEAN--
<?php
$filePath = __DIR__ . "/gh20858.eml";
if (file_exists($filePath)) {
    unlink($filePath);
}
?>
--EXPECTF--
bool(false)
bool(true)
bool(false)
bool(true)
