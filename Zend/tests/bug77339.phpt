--TEST--
Bug #77339 (__callStatic may get incorrect arguments)
--FILE--
<?php
class Foo
{
    static function __callStatic($name, $arguments) {
       if ($name === 'get') {
            if (!isset($arguments[0])) {
                var_dump(['getSomeWhat']);
                var_dump($arguments);
                exit;
            }
        }
        echo "OK\n";
    }

    protected function get ($key) {
        echo "BUG!!!\n";
    }
}

class Bar
{
    static function __callStatic($name, $arguments) {
        echo Foo::get('getSomeWhat');
    }
}

Bar::someUndefinedStaticFunction();
?>
--EXPECT--
OK
