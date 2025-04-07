--TEST--
outer class visibility
--FILE--
<?php

class Outer {
        protected class Inner {}
        public Inner $illegal;

        public function test(): void {
                $this->illegal = new Inner();
        }
}

$x = new Outer();
$x->test();

var_dump($x);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Cannot declare protected class Outer\Inner to a public property in Outer::illegal in %s:%d
Stack trace:
#0 %s(%d): Outer->test()
#1 {main}
  thrown in %s on line %d
