--TEST--
Statics in nested functions & evals.
--FILE--
<?php

static $a = array(7,8,9);

function f1() {
    static $a = array(1,2,3);

    function g1() {
        static $a = array(4,5,6);
        var_dump($a);
    }

    var_dump($a);

}

f1();
g1();
var_dump($a);

eval(' static $b = array(10,11,12); ');

function f2() {
    eval(' static $b = array(1,2,3); ');

    function g2a() {
        eval(' static $b = array(4,5,6); ');
        var_dump($b);
    }

    eval('function g2b() { static $b = array(7, 8, 9); var_dump($b); } ');
    var_dump($b);
}

f2();
g2a();
g2b();
var_dump($b);


eval(' function f3() { static $c = array(1,2,3); var_dump($c); }');
f3();

?>
--EXPECT--
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
