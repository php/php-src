--TEST--
public ReflectionMethod ReflectionMethod::getPrototype ( void );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
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

$reflectionMethod = new ReflectionMethod('HelloWorld', 'sayHelloTo');
var_dump($reflectionMethod->getPrototype());

?>
--EXPECT--
object(ReflectionMethod)#2 (2) {
  ["name"]=>
  string(10) "sayHelloTo"
  ["class"]=>
  string(5) "Hello"
}
