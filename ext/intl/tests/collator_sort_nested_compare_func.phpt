--TEST--
Collator::sort() must not let a nested sort change the running comparator
--EXTENSIONS--
intl
--FILE--
<?php
class Nest {
    public static Collator $coll;
    public static bool $done = false;

    public function __toString(): string {
        if (!self::$done) {
            self::$done = true;
            $inner = ['10', '9', '2'];
            self::$coll->sort($inner, Collator::SORT_NUMERIC);
        }
        return 'm';
    }
}

function names(array $a): array {
    return array_map(static fn($v) => is_object($v) ? get_class($v) : $v, $a);
}

$coll = new Collator('en_US');
Nest::$coll = $coll;

$a = ['20', '3', new Nest(), '100', '9'];
var_dump($coll->sort($a, Collator::SORT_STRING));
var_dump(names($a));

Nest::$done = true;
$b = ['20', '3', new Nest(), '100', '9'];
var_dump($coll->sort($b, Collator::SORT_STRING));
var_dump(names($b));
?>
--EXPECT--
bool(true)
array(5) {
  [0]=>
  string(3) "100"
  [1]=>
  string(2) "20"
  [2]=>
  string(1) "3"
  [3]=>
  string(1) "9"
  [4]=>
  string(4) "Nest"
}
bool(true)
array(5) {
  [0]=>
  string(3) "100"
  [1]=>
  string(2) "20"
  [2]=>
  string(1) "3"
  [3]=>
  string(1) "9"
  [4]=>
  string(4) "Nest"
}
