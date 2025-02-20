--TEST--
reflection
--FILE--
<?php

class Point {
    public class Vertex(public int $x, public int $y);
}

$reflection = new ReflectionClass(Point::Vertex::class);
var_dump($reflection->getProperties());
?>
--EXPECT--
array(2) {
  [0]=>
  object(ReflectionProperty)#2 (2) {
    ["name"]=>
    string(1) "x"
    ["class"]=>
    string(13) "Point::Vertex"
  }
  [1]=>
  object(ReflectionProperty)#3 (2) {
    ["name"]=>
    string(1) "y"
    ["class"]=>
    string(13) "Point::Vertex"
  }
}
