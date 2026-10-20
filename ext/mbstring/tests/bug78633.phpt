--TEST--
Bug #78633 (Heap buffer overflow (read) in mb_eregi)
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists('mb_eregi')) die('skip mb_eregi function not available');
?>
--FILE--
<?php
$res = mb_eregi(".+Isssǰ", ".+Isssǰ");
if (is_bool($res)) {
    echo "ok\n";
} else {
    var_dump($res);
}
?>
--EXPECTF--
Deprecated: Function mb_eregi() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
ok
