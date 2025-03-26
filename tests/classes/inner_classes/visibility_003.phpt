--TEST--
accessing outer protected vars
--FILE--
<?php

class Outer {
        private class Inner {
                public function test(Outer $i) {
                        $i->illegal = $this;
                }
        }
        private Inner $illegal;

        public function test(): void {
                new Inner()->test($this);
        }
}

$x = new Outer();
$x->test();

var_dump($x);

?>
--EXPECT--
object(Outer)#1 (1) {
  ["illegal":"Outer":private]=>
  object(Outer\Inner)#2 (0) {
  }
}
