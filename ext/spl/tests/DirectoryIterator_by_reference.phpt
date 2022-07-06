--TEST--
DirectoryIterator: test that you cannot use iterator with reference
--CREDITS--
Havard Eide <nucleuz@gmail.com>
#PHPTestFest2009 Norway 2009-06-09 \o/
--FILE--
<?php
$it = new DirectoryIterator(__DIR__);
foreach( $it as &$file ) {
    echo $file . "\n";
}
?>
--EXPECTF--
Fatal error: Uncaught Error: An iterator cannot be used with foreach by reference in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
