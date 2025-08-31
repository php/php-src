--TEST--
Bug #55478 (FILTER_VALIDATE_EMAIL fails with internationalized domain name addresses containing >1 -)
--EXTENSIONS--
filter
--FILE--
<?php
$email_address = "test@xn--example--7za.de"; // "example-ä.de"
var_dump(filter_var($email_address, FILTER_VALIDATE_EMAIL));
?>
--EXPECT--
string(24) "test@xn--example--7za.de"
