--TEST--
Bug #72402: _php_mb_regex_ereg_replace_exec - double free
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists('mb_ereg')) die('skip mbregex support not available');
?>
--FILE--
<?php
function throwit() {
    throw new Exception('it');
}
$var10 = "throwit";
try {
    $var14 = mb_ereg_replace_callback("", $var10, "");
} catch(Exception $e) {}
?>
DONE
--EXPECTF--
Deprecated: Function mb_ereg_replace_callback() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
DONE
