--TEST--
__exists: not consulted on a plain read ($obj->prop); only BP_VAR_IS contexts trigger it
--FILE--
<?php

/* __exists is scoped to BP_VAR_IS reads (??, isset(), empty(), nullsafe + ??).
 * A plain read $obj->prop must go straight to __get without consulting
 * __exists, otherwise classes would pay an extra magic call on every read. */

class C {
    public function __exists(string $n): bool {
        echo "  __exists($n)\n";
        return true;
    }
    public function __get(string $n): mixed {
        echo "  __get($n)\n";
        return "value-of-$n";
    }
}

echo "1) Plain read \$c->prop calls __get only:\n";
$c = new C;
$x = $c->foo;
var_dump($x);

echo "\n2) Plain read in assignment + var_dump: __get only:\n";
$c = new C;
$y = $c->bar;
var_dump($y);

echo "\n3) Same property, BP_VAR_IS context: __exists is consulted (contrast):\n";
$c = new C;
var_dump($c->baz ?? 'fb');

?>
--EXPECT--
1) Plain read $c->prop calls __get only:
  __get(foo)
string(12) "value-of-foo"

2) Plain read in assignment + var_dump: __get only:
  __get(bar)
string(12) "value-of-bar"

3) Same property, BP_VAR_IS context: __exists is consulted (contrast):
  __exists(baz)
  __get(baz)
string(12) "value-of-baz"
