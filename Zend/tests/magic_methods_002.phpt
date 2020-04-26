--TEST--
Testing __unset with private visibility
--FILE--
<?php

class foo {
    private function __unset($a) {
        print "unset\n";
    }
}

?>
--EXPECTF--
Warning: The magic method foo::__unset() must have public visibility in %s on line %d
