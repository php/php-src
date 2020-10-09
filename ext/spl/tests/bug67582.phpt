--TEST--
Bug #67582: Cloned SplObjectStorage with overwritten getHash fails offsetExists()
--FILE--
<?php

class MyObjectStorage extends SplObjectStorage {
    // Overwrite getHash() with just some (working) test-method
    public function getHash($object) { return get_class($object); }
}

class TestObject {}

$list = new MyObjectStorage();
$list->attach(new TestObject());

foreach($list as $x) var_dump($list->offsetExists($x));

$list2 = clone $list;
foreach($list2 as $x) var_dump($list2->offsetExists($x));

?>
--EXPECT--
bool(true)
bool(true)
