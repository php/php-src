--TEST--
FILTER_VALIDATE_DOMAIN FILTER_FLAG_HOSTNAME should not be locale dependent
--EXTENSIONS--
filter
--SKIPIF--
<?php  // try to activate a single-byte german locale
if (!setlocale(LC_ALL, 'de_DE', 'de_DE.ISO8859-1', 'de_DE.ISO_8859-1')) {
    print "skip Can't find german locale\n";
}
?>
--FILE--
<?php
var_dump(filter_var('٪', FILTER_VALIDATE_DOMAIN, FILTER_FLAG_HOSTNAME));
setlocale(LC_ALL, 'de_DE', 'de_DE.ISO8859-1', 'de_DE.ISO_8859-1');
var_dump(filter_var('٪', FILTER_VALIDATE_DOMAIN, FILTER_FLAG_HOSTNAME));
?>
--EXPECT--
bool(false)
bool(false)
