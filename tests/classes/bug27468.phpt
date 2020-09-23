--TEST--
Bug #27468 (foreach in __destruct() causes segfault)
--FILE--
<?php
class foo {
    function __destruct() {
        try {
            foreach ($this->x as $x);
        }
        catch (\TypeError $e) {
            echo $e->getMessage(), \PHP_EOL;
        }
    }
}
new foo();
echo 'OK';
?>
--EXPECTF--
Warning: Undefined property: foo::$x in %s on line %d
foreach() argument must be of type array|object, null given
OK
