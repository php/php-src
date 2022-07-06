--TEST--
DirectoryIterator: Test empty value to DirectoryIterator constructor
--CREDITS--
Havard Eide <nucleuz@gmail.com>
#PHPTestFest2009 Norway 2009-06-09 \o/
--FILE--
<?php
try {
    $it = new DirectoryIterator("");
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
--EXPECT--
DirectoryIterator::__construct(): Argument #1 ($directory) cannot be empty
