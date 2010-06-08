--TEST--
Statics work like expected for language-based copy'n'paste. No link between methods from the same trait.
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

class C2 {
   use Counter;
}

$o = new C1();
$o->inc();
$o->inc();

$p = new C2();
$p->inc();
$p->inc();

?>
--EXPECTF--	
1
2
1
2
