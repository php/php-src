--TEST--
Test that using "clone with" with deep cloned properties will result in the property to be cloned twice
--FILE--
<?php

class Bar
{
    public function __construct(
        private readonly int $baz
    ) {
    }

    public function __clone(): void
    {
        echo "Cloning Bar\n";
    }
}

class Foo
{
    public function __construct(
        public readonly Bar $bar
    ) {}

    public function __clone(): void
    {
        echo "Cloning Foo\n";
        $this->bar = clone $this->bar;
    }

    public function withBar(): static
    {
        return clone $this with [
            "bar" => new Bar(2),
        ];
    }
}

$foo = new Foo(new Bar(1));

var_dump($foo->withBar());

?>
--EXPECTF--
Cloning Foo
Cloning Bar
object(Foo)#3 (1) {
  ["bar"]=>
  object(Bar)#5 (1) {
    ["baz":"Bar":private]=>
    int(2)
  }
}
