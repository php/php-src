--TEST--
Check that const exprs are pre-evaluated in new arguments
--FILE--
<?php

class C {
    public function __construct(public $x) {}
}
function test(
    $a = new C(__CLASS__),
    $b = new C(__FUNCTION__),
    $c = new C(x: __FILE__),
) {
    var_dump($a, $b, $c);
}
test();

// Check that nested new works as well.
function test2($p = new C(new C(__FUNCTION__))) {
    var_dump($p);
}
test2();

?>
--EXPECTF--
object(C)#1 (1) {
  ["x"]=>
  string(0) ""
}
object(C)#2 (1) {
  ["x"]=>
  string(4) "test"
}
object(C)#3 (1) {
  ["x"]=>
  string(%d) "%snew_arg_eval.php"
}
object(C)#3 (1) {
  ["x"]=>
  object(C)#2 (1) {
    ["x"]=>
    string(5) "test2"
  }
}
