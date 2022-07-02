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
--EXPECT--
ok
