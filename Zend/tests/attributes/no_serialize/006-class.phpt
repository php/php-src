--TEST--
#[\NoSerialize]: Class.
--FILE--
<?php

#[NoSerialize]
class A {
    public $a = 1;
}

class B extends A {}

echo serialize(["a" => new A, "b" => new B]);

?>
--EXPECTF--
a:2:{s:1:"a";N;s:1:"b";N;}
