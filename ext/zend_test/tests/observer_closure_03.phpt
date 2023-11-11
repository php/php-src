--TEST--
Observer: Observability of closures of builtin functions
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
--FILE--
<?php
var_dump(strlen(...)('test'));
echo 'DONE' . PHP_EOL;
?>
--EXPECTF--
<!-- init '%s' -->
<file '%s'>
  <!-- init strlen() -->
  <strlen>
  </strlen>
  <!-- init var_dump() -->
  <var_dump>
int(4)
  </var_dump>
DONE
</file '%s'>
