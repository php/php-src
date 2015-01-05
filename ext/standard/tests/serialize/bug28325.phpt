--TEST--
Bug #28325 (Problem in serialisation of circular references)
--FILE--
<?php
class a {
	public $b;
}
class b {
	public $c;
}
class c {
	public $d;
}
$a = new a();
$a->b = new b();
$a->b->c = new c();
$a->b->c->d = $a;
var_dump(unserialize(serialize($a)));
?>
--EXPECTF--
object(a)#%d (1) {
  ["b"]=>
  object(b)#%d (1) {
    ["c"]=>
    object(c)#%d (1) {
      ["d"]=>
      *RECURSION*
    }
  }
}
