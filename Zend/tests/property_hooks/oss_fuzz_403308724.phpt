--TEST--
OSS-Fuzz #403308724
--FILE--
<?php
class Base {
    public $y { get => 1; }
}

class Test extends Base {
    public $y {
        get => [new class {
            public $inner {get => __PROPERTY__;}
        }, parent::$y::get()];
    }
}

$test = new Test;
$y = $test->y;
var_dump($y);
var_dump($y[0]->inner);
?>
--EXPECT--
array(2) {
  [0]=>
  object(class@anonymous)#2 (0) {
  }
  [1]=>
  int(1)
}
string(5) "inner"
