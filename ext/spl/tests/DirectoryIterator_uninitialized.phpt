--TEST--
Using an uninitialized DirectoryIterator
--FILE--
<?php

class MyDirectoryIterator extends DirectoryIterator {
    public function __construct() {}
}

$it = new MyDirectoryIterator;
try {
    $it->key();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Object not initialized
