--TEST--
The decorated proprety has not been set
--FILE--
<?php

class DecoratedIterator implements Iterator {
    // Oops, not initialized
    public decorated Iterator $it;
}

$iter = new DecoratedIterator;
try {
    var_dump($iter->valid());
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Typed property DecoratedIterator::$it must not be accessed before initialization
