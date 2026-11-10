--TEST--
Observer: sorting callbacks, including reference returns, retain balanced begin and end events
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.show_output=1
zend_test.observer.observe_all=1
--FILE--
<?php
function compare($a, $b): int {
    return $a <=> $b;
}
function &compareReference($a, $b): int {
    $result = $a <=> $b;
    return $result;
}
// Repeat compare() to check events after its observer handlers have been initialized.
foreach ([
    ['usort', 'compare'],
    ['usort', 'compare'],
    ['uksort', 'compareReference'],
] as [$sort, $callback]) {
    $items = [2 => 2, 1 => 1];
    $sort($items, $callback);
    foreach ($items as $item) { echo $item; }
    echo "\n";
}
?>
--EXPECTF--
<!-- init '%s%eobserver_usort.php' -->
<file '%s%eobserver_usort.php'>
  <!-- init usort() -->
  <usort>
    <!-- init compare() -->
    <compare>
    </compare>
  </usort>
12
  <usort>
    <compare>
    </compare>
  </usort>
12
  <!-- init uksort() -->
  <uksort>
    <!-- init compareReference() -->
    <compareReference>
    </compareReference>
  </uksort>
12
</file '%s%eobserver_usort.php'>
