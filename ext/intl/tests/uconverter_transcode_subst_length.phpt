--TEST--
UConverter::transcode() rejects too long substitution strings
--EXTENSIONS--
intl
--INI--
intl.use_exceptions=false
--FILE--
<?php
$subst = str_repeat('A', 129);

var_dump(UConverter::transcode('abc', 'UTF-8', 'ASCII', ['from_subst' => $subst]));
echo intl_get_error_message(), "\n";
var_dump(UConverter::transcode('abc', 'UTF-8', 'ASCII', ['to_subst' => $subst]));
echo intl_get_error_message(), "\n";
?>
--EXPECT--
bool(false)
UConverter::transcode(): returned error 1: U_ILLEGAL_ARGUMENT_ERROR: U_ILLEGAL_ARGUMENT_ERROR
bool(false)
UConverter::transcode(): returned error 1: U_ILLEGAL_ARGUMENT_ERROR: U_ILLEGAL_ARGUMENT_ERROR
