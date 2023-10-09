--TEST--
Observer: Unused retvals from generators are still observable
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
zend_test.observer.show_return_value=1
--FILE--
<?php
function foo() {
    yield 'I should be observable';
    yield 'Me too!';
}

$gen = foo();
$gen->current();
$gen->next();
$gen->current();

echo 'Done' . PHP_EOL;
?>
--EXPECTF--
<!-- init '%s' -->
<file '%s'>
  <!-- init Generator::current() -->
  <Generator::current>
    <!-- init foo() -->
    <foo>
    </foo:'I should be observable'>
  </Generator::current:'I should be observable'>
  <!-- init Generator::next() -->
  <Generator::next>
    <foo>
    </foo:'Me too!'>
  </Generator::next:NULL>
  <Generator::current>
  </Generator::current:'Me too!'>
Done
</file '%s'>
