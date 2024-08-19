--TEST--
GH-15187: Hooked iterator typed property ref tracking
--FILE--
<?php

class C {
    public $a { set {} }
    public int $b;
    public int $c = 1;
    public $d = 2;
}

$c = new C();

foreach ($c as $k => &$v) {
    var_dump($v);
    if ($k === 'c') {
        try {
            $v = 'foo';
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
    }
    if ($k === 'd') {
        $v = 'foo';
    }
}

var_dump($c);

?>
--EXPECTF--
int(1)
Cannot assign string to reference held by property C::$c of type int
int(2)
object(C)#%d (2) {
  ["b"]=>
  uninitialized(int)
  ["c"]=>
  int(1)
  ["d"]=>
  &string(3) "foo"
}
