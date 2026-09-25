--TEST--
Result type inference verification for simple get is skipped
--FILE--
<?php

class Foo {
    private array $backing = [];

    public array $prop {
        get => $this->backing;
    }

    public function test(): void {
        $prop = $this->prop;
    }
}

$foo = new Foo;
$foo->test();
$foo->test();
echo "Done\n";

?>
--EXPECT--
Done
