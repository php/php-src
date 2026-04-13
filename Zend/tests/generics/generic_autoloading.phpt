--TEST--
Generic class: autoloading triggers for base class name
--FILE--
<?php

$autoloaded = [];
spl_autoload_register(function ($class) use (&$autoloaded) {
    $autoloaded[] = $class;
    if ($class === 'AutoBox') {
        eval('class AutoBox<T> { public function __construct(public T $value) {} public function get(): T { return $this->value; } }');
    }
});

// new with generic args triggers autoload for base class
$b = new AutoBox<int>(42);
echo $b->get() . "\n";
var_dump($b);

// Verify autoloader received the base class name (no generics)
echo "Autoloaded: " . implode(", ", $autoloaded) . "\n";

echo "OK\n";
?>
--EXPECTF--
42
object(AutoBox<int>)#%d (1) {
  ["value"]=>
  int(42)
}
Autoloaded: AutoBox
OK
