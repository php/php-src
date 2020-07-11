--TEST--
SPL: RecursiveDirectoryIterator bug 47534
--FILE--
<?php
$it1 = new RecursiveDirectoryIterator(__DIR__, FileSystemIterator::CURRENT_AS_PATHNAME);
$it1->rewind();
echo gettype($it1->current())."\n";

$it2 = new RecursiveDirectoryIterator(__DIR__);
$it2->rewind();
echo gettype($it2->current())."\n";
--EXPECT--
string
object
