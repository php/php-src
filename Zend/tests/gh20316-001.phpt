--TEST--
GH-20316 001: Assign to ref: non-ref rvalue may be turned into a ref
--FILE--
<?php

class C {
    public string $a = '';
    public $b;
    function __toString() {
        global $c; // turns rvalue into a ref
        return '';
    }
}

for ($i = 0; $i < 2; $i++) {
    $c = new C;
    $c->b = &$c->a;
    $c->b = $c;

    var_dump($c);
    unset($c);
}

?>
--EXPECTF--
object(C)#%d (2) {
  ["a"]=>
  &string(0) ""
  ["b"]=>
  &string(0) ""
}
object(C)#%d (2) {
  ["a"]=>
  &string(0) ""
  ["b"]=>
  &string(0) ""
}
