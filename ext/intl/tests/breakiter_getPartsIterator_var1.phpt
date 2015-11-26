--TEST--
IntlBreakIterator::getPartsIterator(): argument variations
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "pt_PT");

$text = 'foo bar tao';

$it = IntlBreakIterator::createWordInstance(NULL);
$it->setText($text);

var_dump(iterator_to_array($it->getPartsIterator(IntlPartsIterator::KEY_SEQUENTIAL)));
var_dump(iterator_to_array($it->getPartsIterator(IntlPartsIterator::KEY_LEFT)));
var_dump(iterator_to_array($it->getPartsIterator(IntlPartsIterator::KEY_RIGHT)));

?>
==DONE==
--EXPECT--
array(5) {
  [0]=>
  string(3) "foo"
  [1]=>
  string(1) " "
  [2]=>
  string(3) "bar"
  [3]=>
  string(1) " "
  [4]=>
  string(3) "tao"
}
array(5) {
  [0]=>
  string(3) "foo"
  [4]=>
  string(1) " "
  [5]=>
  string(3) "bar"
  [8]=>
  string(1) " "
  [9]=>
  string(3) "tao"
}
array(5) {
  [3]=>
  string(3) "foo"
  [5]=>
  string(1) " "
  [8]=>
  string(3) "bar"
  [9]=>
  string(1) " "
  [12]=>
  string(3) "tao"
}
==DONE==
