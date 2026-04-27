--TEST--
__exists: `??` calls __exists then __get only if __exists returned true
--FILE--
<?php

class C {
    public function __exists(string $n): bool {
        echo "  __exists($n)\n";
        return $n === 'present';
    }
    public function __get(string $n): mixed {
        echo "  __get($n)\n";
        return "value-of-$n";
    }
}

echo "1) `??` when __exists returns true: __exists then __get:\n";
$c = new C;
var_dump($c->present ?? 'fallback');

echo "\n2) `??` when __exists returns false: only __exists, fallback used:\n";
$c = new C;
var_dump($c->absent ?? 'fallback');

?>
--EXPECT--
1) `??` when __exists returns true: __exists then __get:
  __exists(present)
  __get(present)
string(16) "value-of-present"

2) `??` when __exists returns false: only __exists, fallback used:
  __exists(absent)
string(8) "fallback"
