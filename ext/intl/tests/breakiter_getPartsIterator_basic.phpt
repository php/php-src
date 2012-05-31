--TEST--
BreakIterator::getPartsIterator(): basic test
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "pt_PT");

$bi = BreakIterator::createWordInstance('pt');
$pi = $bi->getPartsIterator();
var_dump(get_class($pi));
print_r(iterator_to_array($pi));

$bi->setText("foo bar");
$pi = $bi->getPartsIterator();
print_r(iterator_to_array($pi));
?>
==DONE==
--EXPECT--
string(12) "IntlIterator"
Array
(
)
Array
(
    [0] => foo
    [1] =>  
    [2] => bar
)
==DONE==