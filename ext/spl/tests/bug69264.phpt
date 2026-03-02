--TEST--
Bug #69264 (__debugInfo() ignored while extending SPL classes)
--FILE--
<?php
class MyFileInfo extends SplFileInfo {
    public function __debugInfo(): array {
        return ['child' => 42, 'parent' => count(parent::__debugInfo())];
    }
}

class MyDoublyLinkedList extends SplDoublyLinkedList {
    public function __debugInfo(): array {
        return ['child' => 42, 'parent' => count(parent::__debugInfo())];
    }
}

class MyObjectStorage extends SplObjectStorage {
    public function __debugInfo(): array {
        return ['child' => 42, 'parent' => count(parent::__debugInfo())];
    }
}

class MyMultipleIterator extends MultipleIterator {
    public function __debugInfo(): array {
        return ['child' => 42, 'parent' => count(parent::__debugInfo())];
    }
}

class MyArrayObject extends ArrayObject {
    public function __debugInfo(): array {
        return ['child' => 42, 'parent' => count(parent::__debugInfo())];
    }
}

class MyArrayIterator extends ArrayIterator {
    public function __debugInfo(): array {
        return ['child' => 42, 'parent' => count(parent::__debugInfo())];
    }
}

class MyMaxHeap extends SplMaxHeap {
    public function __debugInfo(): array {
        return ['child' => 42, 'parent' => count(parent::__debugInfo())];
    }
}

class MyPriorityQueue extends SplPriorityQueue {
    public function __debugInfo(): array {
        return ['child' => 42, 'parent' => count(parent::__debugInfo())];
    }
}

var_dump(
    new MyFileInfo(__FILE__),
    new MyDoublyLinkedList(),
    new MyObjectStorage(),
    new MyMultipleIterator(),
    new MyArrayObject(),
    new MyArrayIterator(),
    new MyMaxHeap(),
    new MyPriorityQueue(),
);
?>
--EXPECTF--
object(MyFileInfo)#%d (2) {
  ["child"]=>
  int(42)
  ["parent"]=>
  int(2)
}
object(MyDoublyLinkedList)#%d (2) {
  ["child"]=>
  int(42)
  ["parent"]=>
  int(2)
}
object(MyObjectStorage)#%d (2) {
  ["child"]=>
  int(42)
  ["parent"]=>
  int(1)
}
object(MyMultipleIterator)#%d (2) {
  ["child"]=>
  int(42)
  ["parent"]=>
  int(1)
}
object(MyArrayObject)#%d (2) {
  ["child"]=>
  int(42)
  ["parent"]=>
  int(1)
}
object(MyArrayIterator)#%d (2) {
  ["child"]=>
  int(42)
  ["parent"]=>
  int(1)
}
object(MyMaxHeap)#%d (2) {
  ["child"]=>
  int(42)
  ["parent"]=>
  int(3)
}
object(MyPriorityQueue)#%d (2) {
  ["child"]=>
  int(42)
  ["parent"]=>
  int(3)
}
