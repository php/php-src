--TEST--
Bug #78185: file cache only no longer works
--INI--
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.file_cache={PWD}
opcache.file_cache_only=1
opcache.jit=0
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
if (substr(PHP_OS, 0, 3) !== 'WIN') {
    $pattern = __DIR__ . '/*/' . __DIR__ . '/*78185.php.bin';
} else {
    $pattern = __DIR__ . '/*/*/' . str_replace(':', '', __DIR__) . '/*78185.php.bin';
}
foreach (glob($pattern) as $p) {
    var_dump($p);
}
?>
--CLEAN--
<?php
if (substr(PHP_OS, 0, 3) !== 'WIN') {
    $pattern = __DIR__ . '/*/' . __DIR__ . '/*78185.php.bin';
} else {
    $pattern = __DIR__ . '/*/*/' . str_replace(':', '', __DIR__) . '/*78185.php.bin';
}
foreach (glob($pattern) as $p) {
    unlink($p);
    $p = dirname($p);
    while(strlen($p) > strlen(__DIR__)) {
        rmdir($p);
        $p = dirname($p);
    }
}
?>
--EXPECTF--
string(%d) "%s78185.php.bin"

