--TEST--
GH-20858 Null pointer dereference in php_mail_detect_multiple_crlf via error_log
--INI--
sendmail_path="cat > /tmp/php_test_gh20878.out"
mail.force_extra_parameters="-n"
mail.add_x_header = Off
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == "WIN")
  die("skip Won't run on Windows");
?>
--FILE--
<?php

$headers  = "From: test <mail@domain.tld>\n";
$headers .= "Cc: test <mail@domain.tld>\n"; 
$headers .= 'X-Mailer: PHP/' . phpversion();

// Send mail with nothing set
var_dump(error_log("Error message", 1, null));

// Send mail with destination set
var_dump(error_log("Error message with dest", 1, "default@domain.tld>", null));

// Send mail with custom headers
var_dump(error_log("Error message cust headers", 1, null, $headers));

// Send mail with destination set + custom headers
var_dump(error_log("Error message with both", 1, "default@domain.tld>", $headers));
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
