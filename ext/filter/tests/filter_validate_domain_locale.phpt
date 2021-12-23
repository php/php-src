--TEST--
FILTER_VALIDATE_DOMAIN FILTER_FLAG_HOSTNAME should not be locale dependent
--EXTENSIONS--
filter
--SKIPIF--
<?php  // try to activate a single-byte german locale
if (!setlocale(LC_ALL, "de_DE")) {
    print "skip Can't find german locale";
}
?>
--FILE--
<?php
var_dump(filter_var('٪', FILTER_VALIDATE_DOMAIN, FILTER_FLAG_HOSTNAME));
setlocale(LC_ALL, "de_DE");
var_dump(filter_var('٪', FILTER_VALIDATE_DOMAIN, FILTER_FLAG_HOSTNAME));
?>
--EXPECT--
bool(false)
bool(false)
