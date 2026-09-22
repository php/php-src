--TEST--
GH-17214: Relax final+private warning for trait methods with inherited final
--FILE--
<?php

trait MyTrait
{
    final protected function someMethod(): void {}
}

class Test
{
    use MyTrait {
        someMethod as private anotherMethod;
    }

    public function __construct()
    {
        $this->anotherMethod();
    }
}

?>
===DONE===
--EXPECT--
===DONE===
