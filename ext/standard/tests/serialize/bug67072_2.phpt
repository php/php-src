--TEST--
Bug #67072 Echoing unserialized "SplFileObject" crash - BC break fixes
--FILE--
<?php
class MySplFileObject extends SplFileObject {}
class MyArrayObject extends ArrayObject{ var $a = 1; }
echo unserialize('O:15:"MySplFileObject":1:{s:9:"*filename";s:15:"/home/flag/flag";}');

function testClass($className) 
{
    // simulate phpunit
    $object = unserialize(sprintf('O:%d:"%s":0:{}', strlen($className), $className));
    return $object;
}

class MyClass {}
class MyClassSer implements Serializable {
        function serialize() { return "";}
        function unserialize($data) { }
}
class MyClassSer2 extends MyClassSer {
}

$classes = array('stdClass', 'MyClass', 'MyClassSer', 'MyClassSer2', 'SplFileObject', 'MySplFileObject', 
                 'SplObjectStorage', 'FooBar', 'Closure', 'ArrayObject', 'MyArrayObject',
                 'Directory'
             );
foreach($classes as $cl) {
        var_dump(testClass($cl));
}

?>
===DONE==
--EXPECTF--
Warning: Erroneous data format for unserializing 'MySplFileObject' in %s on line 4

Notice: unserialize(): Error at offset 26 of 66 bytes in %s on line 4
object(stdClass)#%d (0) {
}
object(MyClass)#%d (0) {
}
object(MyClassSer)#%d (0) {
}
object(MyClassSer2)#%d (0) {
}

Warning: Erroneous data format for unserializing 'SplFileObject' in %s on line 9

Notice: unserialize(): Error at offset 24 of 25 bytes in %s on line 9
bool(false)

Warning: Erroneous data format for unserializing 'MySplFileObject' in %s on line 9

Notice: unserialize(): Error at offset 26 of 27 bytes in %s on line 9
bool(false)
object(SplObjectStorage)#%d (1) {
  ["storage":"SplObjectStorage":private]=>
  array(0) {
  }
}
object(__PHP_Incomplete_Class)#%d (1) {
  ["__PHP_Incomplete_Class_Name"]=>
  string(6) "FooBar"
}

Warning: Erroneous data format for unserializing 'Closure' in %s on line 9

Notice: unserialize(): Error at offset 17 of 18 bytes in %s on line 9
bool(false)
object(ArrayObject)#%d (1) {
  ["storage":"ArrayObject":private]=>
  array(0) {
  }
}
object(MyArrayObject)#1 (2) {
  ["a"]=>
  int(1)
  ["storage":"ArrayObject":private]=>
  array(0) {
  }
}
object(Directory)#1 (0) {
}
===DONE==
