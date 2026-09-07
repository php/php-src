--TEST--
PFA support observers
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.show_output=1
zend_test.observer.observe_all=1
--FILE--
<?php

function f($a, $b) {
    var_dump($a, $b);
}

$f = f(1, ...);
$f(2);

?>
--EXPECTF--
<!-- init '%sobservers.php' -->
<file '%sobservers.php'>
  <!-- init {closure:%s}() -->
  <{closure:%s}>
    <!-- init f() -->
    <f>
      <!-- init var_dump() -->
      <var_dump>
int(1)
int(2)
      </var_dump>
    </f>
  </{closure:%s:%d}>
</file '%sobservers.php'>
