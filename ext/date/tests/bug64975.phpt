--TEST--
Test for bug #64975: Error parsing when AM PM not at the end
--CREDITS--
Chris Wright <daverandom@php.net>
--FILE--
<?php

$date = DateTime::createFromFormat('d M Y A h:i', '11 Mar 2013 PM 3:34');
$errors = date_get_last_errors();

var_dump($date, $errors['errors'][12]);

?>
--EXPECT--
bool(false)
string(51) "Meridian can only come after an hour has been found"
