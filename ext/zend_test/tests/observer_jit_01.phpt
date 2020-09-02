--TEST--
Observer: JIT is disabled when observer extension is present
--SKIPIF--
<?php if (!extension_loaded('zend-test')) die('skip: zend-test extension required'); ?>
<?php if (!extension_loaded('Zend OPcache')) die('skip: OPcache extension required'); ?>
<?php if (!isset(opcache_get_status()['jit'])) die('skip: Build with JIT required'); ?>
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
opcache.enable=1
opcache.enable_cli=1
opcache.jit=1
opcache.jit_buffer_size=1M
--FILE--
<?php
$status = opcache_get_status();
echo 'JIT enabled: ' . ($status['jit']['enabled'] ? 'yes' : 'no') . PHP_EOL;
echo 'JIT on: ' . ($status['jit']['on'] ? 'yes' : 'no') . PHP_EOL;
?>
--EXPECTF--
<!-- init '%s/observer_jit_%d.php' -->
<file '%s/observer_jit_%d.php'>
JIT enabled: no
JIT on: no
</file '%s/observer_jit_%d.php'>
