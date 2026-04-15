--TEST--
Bug #76999 (mb_regex_set_options() return current options)
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists('mb_regex_set_options')) die('skip mb_regex_set_options() not available');
?>
--FILE--
<?php
mb_regex_set_options("pr");
var_dump(mb_regex_set_options("m"));
var_dump(mb_regex_set_options("mdi"));
var_dump(mb_regex_set_options("m"));
try {
    var_dump(mb_regex_set_options("a"));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump(mb_regex_set_options());
?>
--EXPECTF--
Deprecated: Function mb_regex_set_options() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d

Deprecated: Function mb_regex_set_options() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(2) "pr"

Deprecated: Function mb_regex_set_options() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(2) "mr"

Deprecated: Function mb_regex_set_options() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(3) "imd"

Deprecated: Function mb_regex_set_options() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
Option "a" is not supported

Deprecated: Function mb_regex_set_options() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(2) "mr"
