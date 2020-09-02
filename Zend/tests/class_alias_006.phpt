--TEST--
Testing creation of alias to an internal class
--FILE--
<?php

try {
    class_alias('stdclass', 'foo');
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
class_alias(): Argument #1 ($class) must be a user-defined class name, internal class name given
