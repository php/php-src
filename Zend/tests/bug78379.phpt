--TEST--
Bug #78379 (Cast to object confuses GC, causes crash)
--FILE--
<?php
class C {
    public $p;
    public function __construct() {
        $this->p = (object)["x" => [1]];
    }
}
class E {
    public $e;
    public $f;
    public $a;
}
$e = new E;
$e->f = new E;
$e->f->e = $e;
$e->a = new C;
$e = null;
gc_collect_cycles();
var_dump(new C);
?>
--EXPECTF--
object(C)#%d (1) {
  ["p"]=>
  object(stdClass)#%d (1) {
    ["x"]=>
    array(1) {
      [0]=>
      int(1)
    }
  }
}
