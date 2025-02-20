--TEST--
Bug #72496 (declare public method with signature incompatible with parent private method should not throw a warning)
--FILE--
<?php
class Foo
{
    private function getFoo()
    {
        return 'Foo';
    }

    private function getBar()
    {
        return 'Bar';
    }

    private function getBaz()
    {
        return 'Baz';
    }
}

class Bar extends Foo
{
    public function getFoo($extraArgument)
    {
        return $extraArgument;
    }

    protected function getBar($extraArgument)
    {
        return $extraArgument;
    }

    private function getBaz($extraArgument)
    {
        return $extraArgument;
    }
}

echo "OK\n";
?>
--EXPECT--
OK
