--TEST--
Bug #72164 (Null Pointer Dereference - mb_ereg_replace)
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists('mb_ereg')) die('skip mbregex support not available');
?>
--FILE--
<?php
$var0 = "e";
$var2 = "";
$var3 = "";
try {
    $var8 = mb_ereg_replace($var2,$var3,$var3,$var0);
    var_dump($var8);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECTF--
Deprecated: Function mb_ereg_replace() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
Option "e" is not supported
