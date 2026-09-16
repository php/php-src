--TEST--
Collator::sort() must not reorder the array when __toString() fails
--EXTENSIONS--
intl
--FILE--
<?php
class Thrower {
    public function __toString(): string {
        throw new Exception('boom');
    }
}

class NoToString {
}

function names(array $a): array {
    return array_map(static fn($v) => is_object($v) ? get_class($v) : $v, $a);
}

$coll = new Collator('en_US');

foreach ([Collator::SORT_REGULAR, Collator::SORT_STRING] as $flag) {
    $array = ['b', new Thrower(), 'a'];
    try {
        var_dump($coll->sort($array, $flag));
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
    var_dump(names($array));
}

$array = ['b', new NoToString(), 'a'];
try {
    var_dump($coll->sort($array, Collator::SORT_STRING));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
var_dump(names($array));
?>
--EXPECT--
Exception: boom
array(3) {
  [0]=>
  string(1) "b"
  [1]=>
  string(7) "Thrower"
  [2]=>
  string(1) "a"
}
Exception: boom
array(3) {
  [0]=>
  string(1) "b"
  [1]=>
  string(7) "Thrower"
  [2]=>
  string(1) "a"
}
Error: Object of class NoToString could not be converted to string
array(3) {
  [0]=>
  string(1) "b"
  [1]=>
  string(10) "NoToString"
  [2]=>
  string(1) "a"
}
