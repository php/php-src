--TEST--
Repeated include must not increase memory - class in if condition
--SKIPIF--
<?php
// TODO test should pass even without opcache, but fixes are needed
// related bug tracker https://bugs.php.net/bug.php?id=76982
if (!extension_loaded('Zend OPcache') || !(opcache_get_status() ?: ['opcache_enabled' => false])['opcache_enabled']) {
    die('xfail test currently requires opcache enabled');
}
?>
--FILE--
<?php

$m = -1;
$mDiff = -1;
$mPrev = 0;
for ($i = 0; $i < (PHP_OS_FAMILY === 'Windows' ? 1_000 /* include is slow on Windows */ : 20_000); $i++) {
    require __DIR__ . '/class_in_if.inc';
    assert(Test::verify() === 'ok');
    $m = memory_get_usage();
    $mDiff = $m - $mPrev;
    if ($mPrev !== 0 && $mDiff !== 0) {
        echo 'Increased memory detected: ' . $mDiff . ' B (' . $i . ' iteration)' . "\n";
        exit(1);
    }
    $mPrev = $m;
}
echo 'done';

?>
--EXPECT--
done
