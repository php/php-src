--TEST--
Promoted readonly property reassignment in constructor - different object fails
--FILE--
<?php

// Constructor can modify its own promoted property, but not another object's
class Foo {
    public function __construct(
        public readonly int $x = 0,
        ?Foo $other = null,
    ) {
        $this->x = $x * 2;
        if ($other !== null) {
            try {
                $other->x = 999;
            } catch (Throwable $e) {
                echo get_class($e), ": ", $e->getMessage(), "\n";
            }
        }
    }
}

$a = new Foo(5);
var_dump($a->x);

$b = new Foo(3, $a);
var_dump($a->x, $b->x);  // $a unchanged

?>
--EXPECT--
int(10)
Error: Cannot modify readonly property Foo::$x
int(10)
int(6)
