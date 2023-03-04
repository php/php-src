--TEST--
Observer: Basic observability of closures
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
--FILE--
<?php
$bar = function() {
    var_dump(array_reduce([40, 2], fn ($a, $b) => $a + $b));
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
      <!-- init array_reduce() -->
      <array_reduce>
        <!-- init {closure}() -->
        <{closure}>
        </{closure}>
        <{closure}>
        </{closure}>
      </array_reduce>
      <!-- init var_dump() -->
      <var_dump>
int(42)
      </var_dump>
    </{closure}>
  </{closure}>
  <{closure}>
Answer
    <{closure}>
      <array_reduce>
        <{closure}>
        </{closure}>
        <{closure}>
        </{closure}>
      </array_reduce>
      <var_dump>
int(42)
      </var_dump>
    </{closure}>
  </{closure}>
  <{closure}>
Answer
    <{closure}>
      <array_reduce>
        <{closure}>
        </{closure}>
        <{closure}>
        </{closure}>
      </array_reduce>
      <var_dump>
int(42)
      </var_dump>
    </{closure}>
  </{closure}>
DONE
</file '%s'>
