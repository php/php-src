--TEST--
Collator::sort() conversion failure must survive a re-entrant Collator call in __toString()
--EXTENSIONS--
intl
--FILE--
<?php
class BadAfterReentry {
    public static Collator $coll;
    public static bool $called = false;

    public function __toString(): string {
        self::$called = true;
        self::$coll->getLocale(Locale::VALID_LOCALE);
        return "\xFF";
    }
}

$coll = new Collator('en_US');
BadAfterReentry::$coll = $coll;

$array = ['a', new BadAfterReentry(), 'b'];

var_dump($coll->sort($array, Collator::SORT_STRING));
var_dump(BadAfterReentry::$called);
var_dump($coll->getErrorCode() === U_INVALID_CHAR_FOUND);
echo $coll->getErrorMessage(), PHP_EOL;
var_dump($array[0], $array[1] instanceof BadAfterReentry, $array[2]);
?>
--EXPECT--
bool(false)
bool(true)
bool(true)
Collator::sort(): Error converting string from UTF-8 to UTF-16: U_INVALID_CHAR_FOUND
string(1) "a"
bool(true)
string(1) "b"
