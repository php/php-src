--TEST--
Object instantiated without parentheses is collected
--FILE--
<?php

class A
{
    public function test(): void
    {
        echo 'called' . PHP_EOL;
    }

    public function __destruct()
    {
        echo 'collected' . PHP_EOL;
    }
}

new A()->test();
echo 'code after' . PHP_EOL;

?>
--EXPECT--
called
collected
code after
