--TEST--
Bug #44487 (call_user_method_array issues a warning when throwing an exception)
--INI--
error_reporting = E_ALL & ~E_DEPRECATED
--FILE--
<?php

class Foo
{
        public function test()
        {
                print 'test';
                throw new Exception();
        }
}

try {
        $bar = new Foo();
        call_user_method_array('test', $bar, array()) ;
} catch (Exception $e) {
}
?>
--EXPECT--
test
