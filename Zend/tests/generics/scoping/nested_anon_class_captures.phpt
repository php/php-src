--TEST--
Scoping: nested anonymous class captures enclosing type parameter
--FILE--
<?php
interface Animal {}
function makeAnimalBox<T : Animal>(T $x): object {
    return new class($x) {
        public T $val;
        public function __construct(T $x) { $this->val = $x; }
        public function get(): T { return $this->val; }
    };
}
class Dog implements Animal {}
$obj = makeAnimalBox(new Dog);
$r = new ReflectionClass($obj);
echo $r->getProperty('val')->getType()->getName(), "\n";
echo $r->getMethod('get')->getReturnType()->getName(), "\n";
?>
--EXPECT--
Animal
Animal
