--TEST--
Bug #72399 (Use-After-Free in MBString (search_re))
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists('mb_ereg')) die('skip mbregex support not available');
?>
--FILE--
<?php
$var5 = mb_ereg_search_init("","2");
var_dump($var5);
$var6 = mb_eregi_replace("2","","");
var_dump($var6);

try {
    $var13 = mb_ereg_search_pos();
} catch (\Error $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
--EXPECTF--
Deprecated: Function mb_ereg_search_init() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_eregi_replace() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(0) ""

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
No pattern was provided
