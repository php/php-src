--TEST--
ReflectionClass::get{Get,Set}() inheritance
--FILE--
<?php

class A {
    public $foo {
        get {
            return 'A::$foo::get';
        }
        set {
            echo "A::\$foo::set\n";
        }
    }
}

class B extends A {
    public $foo {
        get {
            return 'B::$foo';
        }
    }
}

$a = new A();
$b = new B();

echo ((new ReflectionProperty(A::class, 'foo'))->getHook('get')->invoke($a)), "\n";
echo ((new ReflectionProperty(A::class, 'foo'))->getHook('get')->invoke($b)), "\n";
echo ((new ReflectionProperty(B::class, 'foo'))->getHook('get')->invoke($b)), "\n";

((new ReflectionProperty(A::class, 'foo'))->getHook('set')->invoke($a, null));
((new ReflectionProperty(A::class, 'foo'))->getHook('set')->invoke($b, null));
((new ReflectionProperty(B::class, 'foo'))->getHook('set')->invoke($b, null));

?>
--EXPECT--
A::$foo::get
A::$foo::get
B::$foo
A::$foo::set
A::$foo::set
A::$foo::set
