--TEST--
Bug #68128 - RecursiveRegexIterator raises "Array to string conversion" notice
--FILE--
<?php

$arrayIterator = new ArrayIterator(array('key 1' => 'value 1', 'key 2' => ['value 2']));
$regexIterator = new RegexIterator($arrayIterator, '/^key/', RegexIterator::MATCH, RegexIterator::USE_KEY);

foreach ($regexIterator as $key => $value) {
    var_dump($key, $value);
}

?>
--EXPECT--
string(5) "key 1"
string(7) "value 1"
string(5) "key 2"
array(1) {
  [0]=>
  string(7) "value 2"
}
