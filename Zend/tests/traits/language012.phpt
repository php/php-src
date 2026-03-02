--TEST--
Statics should work in traits, too.
--FILE--
<?php
error_reporting(E_ALL);

trait Counter {
   public function inc() {
     static $c = 0;
     $c = $c + 1;
     echo "$c\n";
   }
}


class C1 {
   use Counter;
}

$o = new C1();
$o->inc();
$o->inc();

?>
--EXPECT--
1
2
