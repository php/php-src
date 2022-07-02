--TEST--
Bug #34893 (PHP5.1 overloading, Cannot access private property)
--FILE--
<?php
class A {
    private $p;
    function __get($name){
        return $this->$name;
    }
    function __set($name, $value) {
        $this->$name = $value;
    }
}
class B {
    private $t;
    function __get($name){
        return $this->$name;
    }
    function __set($name, $value) {
        $this->$name = $value;
    }
}
$a = new A;
$b = new B;
$a->p = $b;
$b->t = "foo";

echo $a->p->t;
$a->p->t = "bar";
echo $a->p->t;
?>
--EXPECT--
foobar
