--TEST--
Collator::sort() SORT_REGULAR conversion error cleanup paths
--EXTENSIONS--
intl
--FILE--
<?php
class BadRegularString {
    public function __toString(): string {
        return "\xFF";
    }
}

$coll = new Collator('en_US');

$array = [new BadRegularString(), 1];
var_dump($coll->sort($array, Collator::SORT_REGULAR));
var_dump($array[0] instanceof BadRegularString);
var_dump($array[1]);
var_dump(intl_get_error_code() === U_INVALID_CHAR_FOUND);
echo intl_get_error_message(), "\n";
var_dump($coll->getErrorCode() === U_INVALID_CHAR_FOUND);
echo $coll->getErrorMessage(), "\n";

$array = ['a', new BadRegularString()];
var_dump($coll->sort($array, Collator::SORT_REGULAR));
var_dump(intl_get_error_code() === U_INVALID_CHAR_FOUND);
echo intl_get_error_message(), "\n";
var_dump($coll->getErrorCode() === U_INVALID_CHAR_FOUND);
var_dump($array[0], $array[1] instanceof BadRegularString);

$array = ['10', '9'];
var_dump($coll->sort($array, Collator::SORT_REGULAR));
var_dump($coll->getErrorCode() === U_ZERO_ERROR);
var_dump($array);

$array = ['b', '9'];
var_dump($coll->sort($array, Collator::SORT_REGULAR));
var_dump($coll->getErrorCode() === U_ZERO_ERROR);
var_dump($array);
?>
--EXPECT--
bool(false)
bool(true)
int(1)
bool(true)
Collator::sort(): Error converting object string from UTF-8 to UTF-16: U_INVALID_CHAR_FOUND
bool(true)
Collator::sort(): Error converting object string from UTF-8 to UTF-16: U_INVALID_CHAR_FOUND
bool(false)
bool(true)
Collator::sort(): Error converting object string from UTF-8 to UTF-16: U_INVALID_CHAR_FOUND
bool(true)
string(1) "a"
bool(true)
bool(true)
bool(true)
array(2) {
  [0]=>
  string(1) "9"
  [1]=>
  string(2) "10"
}
bool(true)
bool(true)
array(2) {
  [0]=>
  string(1) "9"
  [1]=>
  string(1) "b"
}
