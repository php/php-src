--TEST--
public ReflectionMethod ReflectionMethod::hasPrototype ( void );
--FILE--
<?php
class Hello {
    public function sayHelloTo($name) {
        return 'Hello ' . $name;
    }
}

class HelloWorld extends Hello {
    public function sayHelloTo($name) {
        return 'Hello world: ' . $name;
    }

    public function sayHiTo($name) {
        return 'Hi: ' . $name;
    }
}

$reflectionMethod1 = new ReflectionMethod('HelloWorld', 'sayHelloTo');
var_dump($reflectionMethod1->hasPrototype());

$reflectionMethod2 = new ReflectionMethod('Hello', 'sayHelloTo');
var_dump($reflectionMethod2->hasPrototype());

$reflectionMethod3 = new ReflectionMethod('HelloWorld', 'sayHiTo');
var_dump($reflectionMethod3->hasPrototype());

?>
--EXPECT--
bool(true)
bool(false)
bool(false)
