--TEST--
Test typed properties binary assign op helper test
--FILE--
<?php
declare(strict_types=1);

class Foo {
    public int $bar = 0;

    public function __construct() {
        $this->bar += 2;
        try {
            $this->bar += 1.5;
        } catch (TypeError $e) {
            echo $e->getMessage(), "\n";
        }
    }
}

$foo = new Foo();

var_dump($foo->bar);
?>
--EXPECT--
Cannot assign float to property Foo::$bar of type int
int(2)
