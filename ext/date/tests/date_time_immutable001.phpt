--TEST--
DateTimeImmutable - invalid arguments
--CREDITS--
Mark Niebergall mbniebergall@gmail.com UPHPU TestFest 2017
--FILE--
<?php
var_dump(date_create_immutable('Invalid'));
?>
--EXPECT--
bool(false)
