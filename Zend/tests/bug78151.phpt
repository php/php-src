--TEST--
Bug #78151 Segfault caused by indirect expressions in PHP 7.4a1
--FILE--
<?php

class Arr
{
    private $foo = '';

    public function __construct(array $array = [])
    {
        $property = 'foo';

        $this->{$property} = &$array[$property];

        \var_dump($this->foo);
    }
}

$arr = new Arr(['foo' => 'bar']);
?>
--EXPECT--
string(3) "bar"
