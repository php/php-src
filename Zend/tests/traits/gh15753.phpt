--TEST--
GH-15753: Overriding readonly properties from traits don't allow assignment from the child
--FILE--
<?php

class P {
    protected readonly int $prop;
}

trait T {
    protected readonly int $prop;
}

class C extends P {
    use T;

    public function __construct() {
        $this->prop = 20;
    }
}

$c = new C();
var_dump($c);

?>
--EXPECT--
object(C)#1 (1) {
  ["prop":protected]=>
  int(20)
}
