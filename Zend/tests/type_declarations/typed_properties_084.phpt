--TEST--
Typed properties and class aliases
--FILE--
<?php
eval(<<<'PHP'
class Foo {}
class_alias('Foo', 'Bar');
PHP);
 
eval(<<<'PHP'
class A {
    public Foo $prop;
}
PHP);

eval(<<<'PHP'
class B extends A {
    public Bar $prop;
}
PHP);

?>
===DONE===
--EXPECT--
===DONE===
