--TEST--
Partial application variation pass
--FILE--
<?php
class Foo {}

try {
    new Foo(?);
} catch (Error $ex) {
    printf("%s\n", $ex->getMessage());
}
?>
--EXPECT--
too many arguments and or place holders for application of Foo::__construct, 1 given and a maximum of 0 expected
