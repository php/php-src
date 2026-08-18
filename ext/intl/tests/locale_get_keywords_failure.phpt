--TEST--
Locale::getKeywords() closes the keyword enumeration on failure
--EXTENSIONS--
intl
--SKIPIF--
<?php
if (version_compare(INTL_ICU_VERSION, '59.1', '<')) {
    die('skip for ICU >= 59.1');
}
?>
--FILE--
<?php
var_dump(Locale::getKeywords('en@foo=bar!'));
var_dump(intl_get_error_code() === U_ILLEGAL_ARGUMENT_ERROR);
?>
--EXPECT--
bool(false)
bool(true)
