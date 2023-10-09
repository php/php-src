--TEST--
Observer: Basic observability of closures
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
opcache.optimization_level=0x7FFFBFFF & ~0x0080
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
<!-- init '%s' -->
<file '%s'>
  <!-- init {closure}() -->
  <{closure}>
Answer
    <!-- init {closure}() -->
    <{closure}>
      <!-- init array_sum() -->
      <array_sum>
      </array_sum>
      <!-- init var_dump() -->
      <var_dump>
int(42)
      </var_dump>
    </{closure}>
  </{closure}>
  <{closure}>
Answer
    <{closure}>
      <array_sum>
      </array_sum>
      <var_dump>
int(42)
      </var_dump>
    </{closure}>
  </{closure}>
  <{closure}>
Answer
    <{closure}>
      <array_sum>
      </array_sum>
      <var_dump>
int(42)
      </var_dump>
    </{closure}>
  </{closure}>
DONE
</file '%s'>
