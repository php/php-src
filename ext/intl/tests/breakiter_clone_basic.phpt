--TEST--
IntlBreakIterator: clone handler
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$bi = new IntlRuleBasedBreakIterator('[\p{Letter}\uFFFD]+;[:number:]+;');
$bi_clone = clone $bi;
var_dump(get_class($bi), get_class($bi_clone));
var_dump($bi == $bi_clone);

$bi->setText('foobar');
$bi_clone = clone $bi;
var_dump(get_class($bi), get_class($bi_clone));
var_dump($bi == $bi_clone);
?>
--EXPECT--
string(26) "IntlRuleBasedBreakIterator"
string(26) "IntlRuleBasedBreakIterator"
bool(true)
string(26) "IntlRuleBasedBreakIterator"
string(26) "IntlRuleBasedBreakIterator"
bool(true)
