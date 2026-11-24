--TEST--
UConverter::setSubstChars() rejects an overlong substitution string
--CREDITS--
Wong Wai Kin
--EXTENSIONS--
intl
--INI--
intl.use_exceptions=false
--FILE--
<?php
$converter = new UConverter('US-ASCII', 'UTF-8');
var_dump($converter->setSubstChars('?'));
var_dump($converter->setSubstChars(str_repeat('X', 257)));
var_dump($converter->getErrorCode() === U_ILLEGAL_ARGUMENT_ERROR);
var_dump($converter->getSubstChars());
var_dump($converter->convert("\xE2\x98\x83"));

$converter = (new ReflectionClass(UConverter::class))->newInstanceWithoutConstructor();
$converter->setSourceEncoding('UTF-8');
var_dump($converter->setSubstChars(str_repeat('X', 128)));
var_dump($converter->getErrorCode() === U_INVALID_STATE_ERROR);
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
string(1) "?"
string(1) "?"
bool(false)
bool(true)
