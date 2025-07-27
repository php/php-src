--TEST--
Collator get invalid attribute
--EXTENSIONS--
intl
--FILE--
<?php

$attr = 12345;

$coll = Collator::create('ru_RU');

var_dump($coll->getAttribute($attr));
var_dump($coll->getErrorMessage());

var_dump(collator_get_attribute($coll, $attr));
var_dump(collator_get_error_message($coll));

?>
--EXPECT--
bool(false)
string(55) "Error getting attribute value: U_ILLEGAL_ARGUMENT_ERROR"
bool(false)
string(55) "Error getting attribute value: U_ILLEGAL_ARGUMENT_ERROR"
