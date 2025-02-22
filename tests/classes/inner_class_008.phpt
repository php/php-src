--TEST--
return type hinting
--FILE--
<?php

class Outer {
    public class Inner(string $message);
    public function test(): Outer::Inner {
        return new Outer::Inner('hello');
    }
}

$o = new Outer;
var_dump($o->test());
?>
--EXPECT--
object(Outer::Inner)#2 (1) {
  ["message"]=>
  string(5) "hello"
}
