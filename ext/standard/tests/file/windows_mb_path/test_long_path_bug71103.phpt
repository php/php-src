--TEST--
Bug #71103 file_exists and is_readable fail silently
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");

?>
--FILE--
<?php

$base = __DIR__;
$d = $base . '\\dev\\http\\tproj\\app\\cache\\dev_old\\annotations\\72';

$foo = $d . '\\5b53796d666f6e795c42756e646c655c5477696742756e646c655c436f6e74726f6c6c65725c457863657074696f6e436f6e74726f6c6c657223676574416e64436c65616e4f7574707574427566666572696e67405b416e6e6f745d5d5b.doctrinecache.data';
$bar = $d . '\\5b53796d666f6e795c42756e646c655c5477696742756e646c655c436f6e74726f6c6c65725c457863657074696f6e436f6e74726f6c6c657223676574416e64436c65616e4f7574707574427566666572696e67405b416e6e6f745d5d5b315d.doctrinecache.data';

mkdir($d, 0777, true);

foreach (array($foo, $bar) as $f) {
    touch($f);

    $foo_obj = new \SplFileInfo($f);
    var_dump(
        $f,
        strlen($f) > 260, /* exceeds _MAX_PATH */
        file_exists($f),
        file_exists($foo_obj),
        is_readable($f),
        is_readable($foo_obj),
        is_writable($f),
        is_writable($foo_obj)
    );

    unlink($f);
}

$p = $d;
do {
    rmdir($p);
    $p = dirname($p);
} while ($p != $base);

?>
--EXPECTF--
string(%d) "%s\dev\http\tproj\app\cache\dev_old\annotations\72\5b53796d666f6e795c42756e646c655c5477696742756e646c655c436f6e74726f6c6c65725c457863657074696f6e436f6e74726f6c6c657223676574416e64436c65616e4f7574707574427566666572696e67405b416e6e6f745d5d5b.doctrinecache.data"
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
string(%d) "%s\dev\http\tproj\app\cache\dev_old\annotations\72\5b53796d666f6e795c42756e646c655c5477696742756e646c655c436f6e74726f6c6c65725c457863657074696f6e436f6e74726f6c6c657223676574416e64436c65616e4f7574707574427566666572696e67405b416e6e6f745d5d5b315d.doctrinecache.data"
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
