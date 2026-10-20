--TEST--
collator_sort() and collator_asort() report conversion errors
--EXTENSIONS--
intl
--FILE--
<?php
class BadProceduralString {
    public function __toString(): string {
        return "\xFF";
    }
}

$coll = collator_create('en_US');

$array = ['b', new BadProceduralString(), 'a'];
var_dump(collator_sort($coll, $array, Collator::SORT_STRING));
var_dump($array[0]);
var_dump($array[1] instanceof BadProceduralString);
var_dump($array[2]);
var_dump(intl_get_error_code() === U_INVALID_CHAR_FOUND);
echo intl_get_error_message(), "\n";
var_dump(collator_get_error_code($coll) === U_INVALID_CHAR_FOUND);
echo collator_get_error_message($coll), "\n";

$array = ['b' => 'b', 'bad' => new BadProceduralString(), 'a' => 'a'];
var_dump(collator_asort($coll, $array, Collator::SORT_STRING));
var_dump(array_keys($array));
var_dump(intl_get_error_code() === U_INVALID_CHAR_FOUND);
echo intl_get_error_message(), "\n";
var_dump(collator_get_error_code($coll) === U_INVALID_CHAR_FOUND);
echo collator_get_error_message($coll), "\n";
?>
--EXPECT--
bool(false)
string(1) "b"
bool(true)
string(1) "a"
bool(true)
collator_sort(): Error converting string from UTF-8 to UTF-16: U_INVALID_CHAR_FOUND
bool(true)
collator_sort(): Error converting string from UTF-8 to UTF-16: U_INVALID_CHAR_FOUND
bool(false)
array(3) {
  [0]=>
  string(1) "b"
  [1]=>
  string(3) "bad"
  [2]=>
  string(1) "a"
}
bool(true)
collator_asort(): Error converting string from UTF-8 to UTF-16: U_INVALID_CHAR_FOUND
bool(true)
collator_asort(): Error converting string from UTF-8 to UTF-16: U_INVALID_CHAR_FOUND
