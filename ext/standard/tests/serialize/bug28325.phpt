--TEST--
Bug #28325 (Problem in serialisation of circular references)
--FILE--
<?php
class a {
	var $b;
}
class b {
	var $c;
}
class c {
	var $d;
}
$a = new a();
$a->b = new b();
$a->b->c = new c();
$a->b->c->d = $a;
var_dump(unserialize(serialize($a)));
?>
--EXPECT--
object(a)(1) {
  ["b"]=>
  object(b)(1) {
    ["c"]=>
    object(c)(1) {
      ["d"]=>
      object(a)(1) {
        ["b"]=>
        object(b)(1) {
          ["c"]=>
          object(c)(1) {
            ["d"]=>
            NULL
          }
        }
      }
    }
  }
}
