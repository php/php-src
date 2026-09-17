--TEST--
Readonly property cannot be reset twice during cloning
--FILE--
<?php

class Foo {
    public function __construct(
        public readonly int $bar
    ) {}

    public function __clone()
    {
        $this->bar = 2;
        var_dump($this);
        $this->bar = 3;
    }
}

$foo = new Foo(1);

try {
    clone $foo;
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

echo "done";

?>
--EXPECT--
object(Foo)#2 (1) {
  ["bar"]=>
  int(2)
}
Error: Cannot modify readonly property Foo::$bar
done
