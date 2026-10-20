--TEST--
Collator::sort() must stop comparing once a conversion has failed
--EXTENSIONS--
intl
--FILE--
<?php
class CountingBad {
    public static int $calls = 0;

    public function __toString(): string {
        self::$calls++;
        return "\xFF";
    }
}

$coll = new Collator('en_US');

foreach ([Collator::SORT_STRING, Collator::SORT_REGULAR] as $flag) {
    CountingBad::$calls = 0;
    $array = ['f', 'e', 'd', new CountingBad(), 'c', 'b', 'a'];
    var_dump($coll->sort($array, $flag));
    var_dump($coll->getErrorCode() === U_INVALID_CHAR_FOUND);
    var_dump(CountingBad::$calls);
}
?>
--EXPECT--
bool(false)
bool(true)
int(1)
bool(false)
bool(true)
int(1)
