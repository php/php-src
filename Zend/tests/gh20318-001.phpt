--TEST--
GH-20318 001: Assign to ref: Ref may be freed by __toString()
--CREDITS--
iluuu1994
--FILE--
<?php

class C {
    public mixed $prop1;
    public ?string $prop2;

    public function __toString() {
        unset($this->prop1);
        unset($this->prop2);
        return 'bar';
    }
}

function test() {
    $c = new C();
    $c->prop1 = 'foo';
    $c->prop1 = &$c->prop2;
    $c->prop1 = $c;
    var_dump($c);
}

test();

?>
==DONE==
--EXPECTF--
object(C)#%d (0) {
  ["prop1"]=>
  uninitialized(mixed)
  ["prop2"]=>
  uninitialized(?string)
}
==DONE==
