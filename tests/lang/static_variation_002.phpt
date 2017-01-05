--TEST--
Static variables in methods & nested functions & evals.
--FILE--
<?php

Class C {
	function f() {
		static $a = array(1,2,3);
		eval(' static $k = array(4,5,6); ');
		
		function cfg() {
			static $a = array(7,8,9);
			eval(' static $k = array(10,11,12); ');
			var_dump($a, $k);
		}
		var_dump($a, $k);
	}
}
$c = new C;
$c->f();
cfg();

Class D {
	static function f() {
		eval('function dfg() { static $b = array(1,2,3); var_dump($b); } ');
	}
}
D::f();
dfg();

eval(' Class E { function f() { static $c = array(1,2,3); var_dump($c); } }');
$e = new E;
$e->f();

?>
--EXPECTF--
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(3) {
  [0]=>
  int(4)
  [1]=>
  int(5)
  [2]=>
  int(6)
}
array(3) {
  [0]=>
  int(7)
  [1]=>
  int(8)
  [2]=>
  int(9)
}
array(3) {
  [0]=>
  int(10)
  [1]=>
  int(11)
  [2]=>
  int(12)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
