--TEST--
Bug #78633 (Heap buffer overflow (read) in mb_eregi)
--SKIPIF--
<?php
if (!extension_loaded('mbstring')) die('skip mbstring extension not available');
if (!function_exists('mb_eregi')) die('skip mb_eregi function not available');
?>
--FILE--
<?php
$res = mb_eregi(".+Isssǰ", ".+Isssǰ");
if ($res === 1 || $res === false) {
    echo "ok\n";
} else {
    var_dump($res);
}
?>
--EXPECT--
ok
