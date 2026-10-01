--TEST--
Collator::sort() conversion error messages describe the failed conversion
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

$array = ['a', new BadString()];
var_dump($coll->sort($array, Collator::SORT_REGULAR));
echo $coll->getErrorMessage(), PHP_EOL;

$array = ['a', new BadString()];
var_dump($coll->sort($array, Collator::SORT_STRING));
echo $coll->getErrorMessage(), PHP_EOL;
?>
--EXPECT--
bool(false)
Collator::sort(): Error converting object string from UTF-8 to UTF-16: U_INVALID_CHAR_FOUND
bool(false)
Collator::sort(): Error converting string from UTF-8 to UTF-16: U_INVALID_CHAR_FOUND
