--TEST--
__exists: isset() distinguishes "missing" from "set to null" (and is `??`-equivalent)
--FILE--
<?php

/* With __exists, isset() must call __get when __exists returns true,
 * so it can apply the standard isset() semantics ("set and not null").
 * This restores the documented equivalence:
 *   isset($x) ? $x : y    is equivalent to    $x ?? y
 */

class C {
    private array $store;
    public function __construct(array $store) { $this->store = $store; }
    public function __exists(string $n): bool {
        echo "  __exists($n)\n";
        return array_key_exists($n, $this->store);
    }
    public function __get(string $n): mixed {
        echo "  __get($n)\n";
        return $this->store[$n] ?? null;
    }
}

echo "1) isset() when __exists=false: only __exists, returns false:\n";
$c = new C([]);
var_dump(isset($c->any));

echo "\n2) isset() when __exists=true and __get returns non-null: returns true:\n";
$c = new C(['x' => 5]);
var_dump(isset($c->x));

echo "\n3) isset() when __exists=true and __get returns null: returns false (matches normal isset):\n";
$c = new C(['x' => null]);
var_dump(isset($c->x));

echo "\n4) `??` agrees with isset() in all three cases:\n";
$c = new C([]);                var_dump($c->any ?? 'fallback');
$c = new C(['x' => 5]);        var_dump($c->x ?? 'fallback');
$c = new C(['x' => null]);     var_dump($c->x ?? 'fallback');

?>
--EXPECT--
1) isset() when __exists=false: only __exists, returns false:
  __exists(any)
bool(false)

2) isset() when __exists=true and __get returns non-null: returns true:
  __exists(x)
  __get(x)
bool(true)

3) isset() when __exists=true and __get returns null: returns false (matches normal isset):
  __exists(x)
  __get(x)
bool(false)

4) `??` agrees with isset() in all three cases:
  __exists(any)
string(8) "fallback"
  __exists(x)
  __get(x)
int(5)
  __exists(x)
  __get(x)
string(8) "fallback"
