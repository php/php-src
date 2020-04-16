--TEST--
DirectoryIterator: Test empty value to DirectoryIterator constructor
--CREDITS--
Havard Eide <nucleuz@gmail.com>
#PHPTestFest2009 Norway 2009-06-09 \o/
--FILE--
<?php

try {
    new DirectoryIterator('');
} catch (\ValueError $ex) {
    echo $ex->getMessage() . PHP_EOL;
}

?>
--EXPECT--
DirectoryIterator::__construct(): Argument #1 ($path) cannot be empty
