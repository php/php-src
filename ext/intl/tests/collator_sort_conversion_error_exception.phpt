--TEST--
Collator::sort() throws IntlException on conversion errors with intl.use_exceptions
--EXTENSIONS--
intl
--INI--
intl.use_exceptions=1
--FILE--
<?php
class BadString {
    public function __toString(): string {
        return "\xFF";
    }
}

$coll = new Collator('en_US');

$array = ['b', new BadString(), 'a'];
try {
    $coll->sort($array, Collator::SORT_STRING);
    echo 'no exception', PHP_EOL;
} catch (IntlException $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
var_dump($coll->getErrorCode() === U_INVALID_CHAR_FOUND);
var_dump($array[0], $array[1] instanceof BadString, $array[2]);

$array = ['b' => 'b', 'bad' => new BadString(), 'a' => 'a'];
try {
    collator_asort($coll, $array, Collator::SORT_STRING);
    echo 'no exception', PHP_EOL;
} catch (IntlException $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
var_dump(array_keys($array));
?>
--EXPECT--
IntlException: Collator::sort(): Error converting string from UTF-8 to UTF-16
bool(true)
string(1) "b"
bool(true)
string(1) "a"
IntlException: collator_asort(): Error converting string from UTF-8 to UTF-16
array(3) {
  [0]=>
  string(1) "b"
  [1]=>
  string(3) "bad"
  [2]=>
  string(1) "a"
}
