--TEST--
outer class visibility
--FILE--
<?php

class Outer {
        protected class Inner {}
        public Outer:>Inner $illegal;

        public function test(): void {
                $this->illegal = new Outer:>Inner();
        }
}

$x = new Outer();
$x->test();

var_dump($x);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Cannot assign protected Outer:>Inner to higher visibile property Outer::illegal in %s:%d
Stack trace:
#0 %s(%d): Outer->test()
#1 {main}
  thrown in %s on line %d
