--TEST--
Observer: Basic observability of closures
--SKIPIF--
<?php if (!extension_loaded('zend-test')) die('skip: zend-test extension required'); ?>
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
--FILE--
<?php
$bar = function() {
    var_dump(array_sum([40,2]));
};

$foo = function($bar) {
    echo 'Answer' . PHP_EOL;
    $bar();
};

$foo($bar);
$foo($bar);
$foo($bar);

echo 'DONE' . PHP_EOL;
?>
--EXPECTF--
<!-- init '%s%eobserver_closure_%d.php' -->
<file '%s%eobserver_closure_%d.php'>
  <!-- init {closure}() -->
  <{closure}>
Answer
    <!-- init {closure}() -->
    <{closure}>
int(42)
    </{closure}>
  </{closure}>
  <{closure}>
Answer
    <{closure}>
int(42)
    </{closure}>
  </{closure}>
  <{closure}>
Answer
    <{closure}>
int(42)
    </{closure}>
  </{closure}>
DONE
</file '%s%eobserver_closure_%d.php'>
