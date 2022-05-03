--TEST--
public ReflectionMethod ReflectionMethod::hasPrototype ( void );
--CREDITS--
ollieread - <code@ollie.codes>
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
}

$reflectionMethod1 = new ReflectionMethod('HelloWorld', 'sayHelloTo');
var_dump($reflectionMethod1->hasPrototype());

$reflectionMethod2 = new ReflectionMethod('Hello', 'sayHelloTo');
var_dump($reflectionMethod2->hasPrototype());

?>
--EXPECT--
bool(true)
bool(false)
