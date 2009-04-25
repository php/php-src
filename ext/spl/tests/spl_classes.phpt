--TEST--
SPL: spl_classes() function 
--CREDITS--
Sebastian Schürmann
sebs@php.net
Testfest 2009 Munich
--FILE--
<?php
var_dump(spl_classes());
?>
--EXPECT--
array(54) {
  ["AppendIterator"]=>
  string(14) "AppendIterator"
  ["ArrayIterator"]=>
  string(13) "ArrayIterator"
  ["ArrayObject"]=>
  string(11) "ArrayObject"
  ["BadFunctionCallException"]=>
  string(24) "BadFunctionCallException"
  ["BadMethodCallException"]=>
  string(22) "BadMethodCallException"
  ["CachingIterator"]=>
  string(15) "CachingIterator"
  ["Countable"]=>
  string(9) "Countable"
  ["DirectoryIterator"]=>
  string(17) "DirectoryIterator"
  ["DomainException"]=>
  string(15) "DomainException"
  ["EmptyIterator"]=>
  string(13) "EmptyIterator"
  ["FilesystemIterator"]=>
  string(18) "FilesystemIterator"
  ["FilterIterator"]=>
  string(14) "FilterIterator"
  ["GlobIterator"]=>
  string(12) "GlobIterator"
  ["InfiniteIterator"]=>
  string(16) "InfiniteIterator"
  ["InvalidArgumentException"]=>
  string(24) "InvalidArgumentException"
  ["IteratorIterator"]=>
  string(16) "IteratorIterator"
  ["LengthException"]=>
  string(15) "LengthException"
  ["LimitIterator"]=>
  string(13) "LimitIterator"
  ["LogicException"]=>
  string(14) "LogicException"
  ["MultipleIterator"]=>
  string(16) "MultipleIterator"
  ["NoRewindIterator"]=>
  string(16) "NoRewindIterator"
  ["OuterIterator"]=>
  string(13) "OuterIterator"
  ["OutOfBoundsException"]=>
  string(20) "OutOfBoundsException"
  ["OutOfRangeException"]=>
  string(19) "OutOfRangeException"
  ["OverflowException"]=>
  string(17) "OverflowException"
  ["ParentIterator"]=>
  string(14) "ParentIterator"
  ["RangeException"]=>
  string(14) "RangeException"
  ["RecursiveArrayIterator"]=>
  string(22) "RecursiveArrayIterator"
  ["RecursiveCachingIterator"]=>
  string(24) "RecursiveCachingIterator"
  ["RecursiveDirectoryIterator"]=>
  string(26) "RecursiveDirectoryIterator"
  ["RecursiveFilterIterator"]=>
  string(23) "RecursiveFilterIterator"
  ["RecursiveIterator"]=>
  string(17) "RecursiveIterator"
  ["RecursiveIteratorIterator"]=>
  string(25) "RecursiveIteratorIterator"
  ["RecursiveRegexIterator"]=>
  string(22) "RecursiveRegexIterator"
  ["RecursiveTreeIterator"]=>
  string(21) "RecursiveTreeIterator"
  ["RegexIterator"]=>
  string(13) "RegexIterator"
  ["RuntimeException"]=>
  string(16) "RuntimeException"
  ["SeekableIterator"]=>
  string(16) "SeekableIterator"
  ["SplDoublyLinkedList"]=>
  string(19) "SplDoublyLinkedList"
  ["SplFileInfo"]=>
  string(11) "SplFileInfo"
  ["SplFileObject"]=>
  string(13) "SplFileObject"
  ["SplFixedArray"]=>
  string(13) "SplFixedArray"
  ["SplHeap"]=>
  string(7) "SplHeap"
  ["SplMinHeap"]=>
  string(10) "SplMinHeap"
  ["SplMaxHeap"]=>
  string(10) "SplMaxHeap"
  ["SplObjectStorage"]=>
  string(16) "SplObjectStorage"
  ["SplObserver"]=>
  string(11) "SplObserver"
  ["SplPriorityQueue"]=>
  string(16) "SplPriorityQueue"
  ["SplQueue"]=>
  string(8) "SplQueue"
  ["SplStack"]=>
  string(8) "SplStack"
  ["SplSubject"]=>
  string(10) "SplSubject"
  ["SplTempFileObject"]=>
  string(17) "SplTempFileObject"
  ["UnderflowException"]=>
  string(18) "UnderflowException"
  ["UnexpectedValueException"]=>
  string(24) "UnexpectedValueException"
}


