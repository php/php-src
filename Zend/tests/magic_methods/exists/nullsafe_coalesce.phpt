--TEST--
__exists: nullsafe + ?? follows the same sequence as ??
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

echo "1) Nullsafe + ?? when __exists=true: __exists then __get:\n";
$c = new C;
var_dump($c?->present ?? 'fallback');

echo "\n2) Nullsafe + ?? when __exists=false: only __exists, fallback used:\n";
$c = new C;
var_dump($c?->absent ?? 'fallback');

echo "\n3) Nullsafe + ?? on a null receiver short-circuits before __exists:\n";
$c = null;
var_dump($c?->present ?? 'fallback');

?>
--EXPECT--
1) Nullsafe + ?? when __exists=true: __exists then __get:
  __exists(present)
  __get(present)
string(16) "value-of-present"

2) Nullsafe + ?? when __exists=false: only __exists, fallback used:
  __exists(absent)
string(8) "fallback"

3) Nullsafe + ?? on a null receiver short-circuits before __exists:
string(8) "fallback"
