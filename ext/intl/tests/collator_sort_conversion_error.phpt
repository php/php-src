--TEST--
Collator::sort() reports conversion errors from comparison callbacks
--EXTENSIONS--
intl
--FILE--
<?php
class BadString {
    public function __toString(): string {
        return "\xFF";
    }
}

$coll = new Collator('en_US');
$array = ['b', new BadString(), 'a'];

var_dump($coll->sort($array, Collator::SORT_STRING));
var_dump(intl_get_error_code() === U_INVALID_CHAR_FOUND);
echo intl_get_error_message(), "\n";
var_dump($coll->getErrorCode() === U_INVALID_CHAR_FOUND);
echo $coll->getErrorMessage(), "\n";
?>
--EXPECT--
bool(false)
bool(true)
Collator::sort(): Error converting string from UTF-8 to UTF-16: U_INVALID_CHAR_FOUND
bool(true)
Collator::sort(): Error converting string from UTF-8 to UTF-16: U_INVALID_CHAR_FOUND
