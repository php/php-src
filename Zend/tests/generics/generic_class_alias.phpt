--TEST--
Generic class: class_alias works with generic classes
--FILE--
<?php

class Box<T> {
    public function __construct(public T $value) {}
    public function get(): T { return $this->value; }
}

class_alias('Box', 'Container');

// Alias with generic args
$c = new Container<int>(42);
echo $c->get() . "\n";
var_dump($c);

// get_class returns the original class name
echo get_class($c) . "\n";

// instanceof works
echo ($c instanceof Box) ? "instanceof Box: yes\n" : "instanceof Box: no\n";
echo ($c instanceof Container) ? "instanceof Container: yes\n" : "instanceof Container: no\n";

// Type enforcement works through alias
try {
    $c->value = "not an int";
} catch (TypeError $e) {
    echo "TypeError: type enforced\n";
}

echo "OK\n";
?>
--EXPECTF--
42
object(Box<int>)#1 (1) {
  ["value"]=>
  int(42)
}
Box
instanceof Box: yes
instanceof Container: yes
TypeError: type enforced
OK
