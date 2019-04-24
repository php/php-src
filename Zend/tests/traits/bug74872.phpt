--TEST--
Bug #74872 (First Trait wins on importing same methods with diff alias)
--FILE--
<?php

trait Trait1
{
    public function init()
    {
        echo("Trait1 - init\n");
    }
}

trait Trait2
{
    public function init()
    {
        echo("Trait2 - init\n");
    }
}

class Test
{
    use Trait1 {
        init as public method1trait1;
    }

    use Trait2 {
        init as public method1trait2;
    }

    final public function __construct()
    {
        $this->init();
        $this->method1trait1();
        $this->method1trait2();
    }

    public function init()
    {
        echo("Test - init\n");
    }
}

$test = new Test();

$reflection = new ReflectionClass( $test );

foreach($reflection->getTraitAliases() as $k => $v)
{
    echo $k.' => '.$v."\n";
}
?>
--EXPECT--
Test - init
Trait1 - init
Trait2 - init
method1trait1 => Trait1::init
method1trait2 => Trait2::init
