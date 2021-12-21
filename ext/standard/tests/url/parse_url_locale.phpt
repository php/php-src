--TEST--
Bug #52923 (Locale settings affecting parse_url)
--SKIPIF--
<?php  // try to activate a german locale
if (!setlocale(LC_ALL, "de_DE")) {
    print "skip Can't find german locale";
}
?>
--FILE--
<?php
echo http_build_query(parse_url("http\xfc://invalid"), true), "\n";
// activate the german locale. With this bug fix, locale settings should no longer affect parse_url
var_dump(setlocale(LC_CTYPE, "de_DE"));
echo http_build_query(parse_url("http\xfc://invalid"), true), "\n";
echo http_build_query(parse_url('http://mydomain.com/path/道')), "\n";
?>
--EXPECT--
path=http%FC%3A%2F%2Finvalid
string(5) "de_DE"
path=http%FC%3A%2F%2Finvalid
scheme=http&host=mydomain.com&path=%2Fpath%2F%E9%81%93
