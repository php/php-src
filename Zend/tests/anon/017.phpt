--TEST--
Anonymous class in trait must be declared only once
--FILE--
<?php

trait TraitWithAnonymousClass
{
    public function foo(): object
    {
        return new class() {};
    }
}

$c = count(get_declared_classes());

if (time() > 0) {
    class A { use TraitWithAnonymousClass; }
    var_dump(count(get_declared_classes()) - $c);
    
    $o = new A();
    var_dump(count(get_declared_classes()) - $c);
    
    $o->foo();
    var_dump(count(get_declared_classes()) - $c);
}

eval('class B { use TraitWithAnonymousClass; }');
var_dump(count(get_declared_classes()) - $c);

$o = new B();
var_dump(count(get_declared_classes()) - $c);

$o->foo();
var_dump(count(get_declared_classes()) - $c);

?>
--EXPECT--
int(1)
int(1)
int(1)
int(2)
int(2)
int(2)
