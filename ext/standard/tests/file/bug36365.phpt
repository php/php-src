--TEST--
Bug #36365 (scandir duplicates file name at every 65535th file)
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die('skip slow test');
?>
--FILE--
<?php
$testdir = __DIR__ . '/bug36365';

mkdir($testdir);
for ($i = 0; $i < 70000; $i++) {
    touch(sprintf("$testdir/%05d.txt", $i));
}

var_dump(count(scandir($testdir)));
?>
--CLEAN--
<?php
$testdir = __DIR__ . '/bug36365';
for ($i = 0; $i < 70000; $i++) {
    unlink(sprintf("$testdir/%05d.txt", $i));
}
rmdir($testdir);
?>
--EXPECT--
int(70002)
