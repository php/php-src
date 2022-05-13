--TEST--
Bug #70453 (IntlChar::foldCase() incorrect arguments and missing constants)
--EXTENSIONS--
intl
--FILE--
<?php
$method = new ReflectionMethod('IntlChar', 'foldCase');
$param = $method->getParameters()[1];
var_dump($param->name, $param->isOptional(), $param->isPassedByReference());
var_dump(IntlChar::foldCase('I', IntlChar::FOLD_CASE_DEFAULT));
var_dump(IntlChar::foldCase('I', IntlChar::FOLD_CASE_EXCLUDE_SPECIAL_I));
?>
--EXPECT--
string(7) "options"
bool(true)
bool(false)
string(1) "i"
string(2) "ı"
