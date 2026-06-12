--TEST--
__exists: without __get, a true return falls through to default property access (warning + null)
--FILE--
<?php

class C {
    public function __exists(string $n): bool {
        echo "  __exists($n)\n";
        return true;
    }
}

echo "1) `??` with __exists=true and no __get: returns null, so the fallback is used:\n";
$c = new C;
var_dump($c->x ?? 'fb');

echo "\n2) __exists=true, no __get, but __exists materialized the property:\n";
class E {
    public int $x = 0;
    public function __exists(string $n): bool {
        echo "  __exists($n)\n";
        $this->$n = 7;
        return true;
    }
}
$e = new E;
unset($e->x);
var_dump($e->x ?? 'fb');

?>
--EXPECTF--
1) `??` with __exists=true and no __get: returns null, so the fallback is used:
  __exists(x)
string(2) "fb"

2) __exists=true, no __get, but __exists materialized the property:
  __exists(x)
int(7)
