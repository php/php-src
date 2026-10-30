--TEST--
GH-15438: Promoted properties with hooks but no visibility
--FILE--
<?php

class C {
    public function __construct(
        $prop { set => $value * 2; }
    ) {}
}

$c = new C(42);
var_dump($c);

?>
--EXPECTF--
object(C)#%d (1) {
  ["prop"]=>
  int(84)
}
