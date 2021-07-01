--TEST--
IntlBreakIterator::getPartsIterator(): basic test, ICU <= 57.1
--EXTENSIONS--
intl
--SKIPIF--
<?php
if (version_compare(INTL_ICU_VERSION, '57.1') > 0) die('skip for ICU <= 57.1');
?>
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "pt_PT");

$bi = IntlBreakIterator::createWordInstance('pt');
$pi = $bi->getPartsIterator();
var_dump(get_class($pi));
print_r(iterator_to_array($pi));

$bi->setText("foo bar");
$pi = $bi->getPartsIterator();
var_dump(get_class($pi->getBreakIterator()));
print_r(iterator_to_array($pi));
var_dump($pi->getRuleStatus());
?>
--EXPECT--
string(17) "IntlPartsIterator"
Array
(
)
string(26) "IntlRuleBasedBreakIterator"
Array
(
    [0] => foo
    [1] =>  
    [2] => bar
)
int(0)
