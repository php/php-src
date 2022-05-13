--TEST--
Bug #76556 (get_debug_info handler for BreakIterator shows wrong type)
--EXTENSIONS--
intl
--FILE--
<?php
$it = IntlBreakIterator::createCharacterInstance();
$it->setText('foo');
var_dump($it);
?>
--EXPECTF--
object(IntlRuleBasedBreakIterator)#%d (3) {
  ["valid"]=>
  bool(true)
  ["text"]=>
  string(3) "foo"
  ["type"]=>
  string(%d) "%SRuleBasedBreakIterator%S"
}
