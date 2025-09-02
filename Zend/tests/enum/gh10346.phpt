--TEST--
GH-10346 (Observer: enum tryFrom() run_time_cache properly assigned)
--CREDITS--
Florian Sowade
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.show_output=1
zend_test.observer.observe_all=1
--FILE--
<?php
enum Card : string
{
    case HEART = 'H';
}

var_dump(Card::tryFrom('H'));
?>
--EXPECTF--
<!-- init '%s' -->
<file '%s'>
  <!-- init Card::tryFrom() -->
  <Card::tryFrom>
  </Card::tryFrom>
  <!-- init var_dump() -->
  <var_dump>
enum(Card::HEART)
  </var_dump>
</file '%s'>
