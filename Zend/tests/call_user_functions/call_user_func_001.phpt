--TEST--
Testing call_user_func inside namespace
--FILE--
<?php

namespace testing {
    function foobar($str) {
        var_dump($str);
    }

    abstract class bar {
        protected function prot($str) {
            print "Shouldn't be called!\n";
        }
    }
    class foo extends bar {
        private function priv($str) {
            print "Shouldn't be called!\n";
        }
    }

    call_user_func(__NAMESPACE__ .'\foobar', 'foobar');

    $class =  __NAMESPACE__ .'\foo';
    try {
        call_user_func(array(new $class, 'priv'), 'foobar');
    } catch (\TypeError $e) {
        echo $e->getMessage(), "\n";
    }
    try {
        call_user_func(array(new $class, 'prot'), 'foobar');
    } catch (\TypeError $e) {
        echo $e->getMessage(), "\n";
    }
}

?>
--EXPECT--
string(6) "foobar"
call_user_func(): Argument #1 ($callback) must be a valid callback, cannot access private method testing\foo::priv()
call_user_func(): Argument #1 ($callback) must be a valid callback, cannot access protected method testing\foo::prot()
