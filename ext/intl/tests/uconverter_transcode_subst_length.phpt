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
var_dump(intl_get_error_code());
var_dump(UConverter::transcode('abc', 'UTF-8', 'ASCII', ['to_subst' => $subst]));
var_dump(intl_get_error_code());
?>
--EXPECT--
bool(false)
int(1)
bool(false)
int(1)
