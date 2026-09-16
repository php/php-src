--TEST--
Collator::sort() and asort() SORT_NUMERIC paths
--EXTENSIONS--
intl
--FILE--
<?php
$coll = new Collator('en_US');

$array = ['10', '-3', '4.5', '2'];
var_dump($coll->sort($array, Collator::SORT_NUMERIC));
var_dump($coll->getErrorCode() === U_ZERO_ERROR);
var_dump($array);

$array = ['ten' => '10', 'minus' => '-3', 'four' => '4.5', 'two' => '2'];
var_dump($coll->asort($array, Collator::SORT_NUMERIC));
var_dump($coll->getErrorCode() === U_ZERO_ERROR);
var_dump(array_keys($array));

$array = ["\xFF", '10', '1'];
var_dump($coll->sort($array, Collator::SORT_NUMERIC));
var_dump($array[0] === "\xFF");
var_dump($array[1], $array[2]);
var_dump(intl_get_error_code() === U_INVALID_CHAR_FOUND);
echo intl_get_error_message(), "\n";
var_dump($coll->getErrorCode() === U_INVALID_CHAR_FOUND);
echo $coll->getErrorMessage(), "\n";

$array = ['ten' => '10', 'bad' => "\xFF", 'one' => '1'];
var_dump($coll->asort($array, Collator::SORT_NUMERIC));
var_dump(array_keys($array));
var_dump($array['bad'] === "\xFF");
var_dump(intl_get_error_code() === U_INVALID_CHAR_FOUND);
echo intl_get_error_message(), "\n";
var_dump($coll->getErrorCode() === U_INVALID_CHAR_FOUND);
echo $coll->getErrorMessage(), "\n";
?>
--EXPECT--
bool(true)
bool(true)
array(4) {
  [0]=>
  string(2) "-3"
  [1]=>
  string(1) "2"
  [2]=>
  string(3) "4.5"
  [3]=>
  string(2) "10"
}
bool(true)
bool(true)
array(4) {
  [0]=>
  string(5) "minus"
  [1]=>
  string(3) "two"
  [2]=>
  string(4) "four"
  [3]=>
  string(3) "ten"
}
bool(false)
bool(true)
string(2) "10"
string(1) "1"
bool(true)
Collator::sort(): Error converting hash from UTF-8 to UTF-16: U_INVALID_CHAR_FOUND
bool(true)
Collator::sort(): Error converting hash from UTF-8 to UTF-16: U_INVALID_CHAR_FOUND
bool(false)
array(3) {
  [0]=>
  string(3) "ten"
  [1]=>
  string(3) "bad"
  [2]=>
  string(3) "one"
}
bool(true)
bool(true)
Collator::asort(): Error converting hash from UTF-8 to UTF-16: U_INVALID_CHAR_FOUND
bool(true)
Collator::asort(): Error converting hash from UTF-8 to UTF-16: U_INVALID_CHAR_FOUND
