--TEST--
Bug #50158 (FILTER_VALIDATE_EMAIL fails with valid addresses containing = or ?)
--EXTENSIONS--
filter
--FILE--
<?php

$email_address = "test=mail@example.com";
var_dump(filter_var($email_address, FILTER_VALIDATE_EMAIL));

$email_address = "test-mail@example.com";
var_dump(filter_var($email_address, FILTER_VALIDATE_EMAIL));

$email_address = "test+mail@example.com";
var_dump(filter_var($email_address, FILTER_VALIDATE_EMAIL));

$email_address = "test?mail@example.com";
var_dump(filter_var($email_address, FILTER_VALIDATE_EMAIL));

?>
--EXPECT--
string(21) "test=mail@example.com"
string(21) "test-mail@example.com"
string(21) "test+mail@example.com"
string(21) "test?mail@example.com"
