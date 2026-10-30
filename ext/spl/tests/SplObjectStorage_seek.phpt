--TEST--
SplObjectStorage::seek() basic functionality
--FILE--
<?php

class Test {
    public function __construct(public string $marker) {}
}

$a = new Test("a");
$b = new Test("b");
$c = new Test("c");
$d = new Test("d");
$e = new Test("e");

$storage = new SplObjectStorage();
$storage[$a] = 1;
$storage[$b] = 2;
$storage[$c] = 3;
$storage[$d] = 4;
$storage[$e] = 5;

echo "--- Error cases ---\n";

try {
    $storage->seek(-1);
} catch (OutOfBoundsException $e) {
    echo $e->getMessage(), "\n";
}
try {
    $storage->seek(5);
} catch (OutOfBoundsException $e) {
    echo $e->getMessage(), "\n";
}

var_dump($storage->key());
var_dump($storage->current());

echo "--- Normal cases ---\n";

$storage->seek(2);
var_dump($storage->key());
var_dump($storage->current());

$storage->seek(1);
var_dump($storage->key());
var_dump($storage->current());

$storage->seek(4);
var_dump($storage->key());
var_dump($storage->current());

$storage->seek(0);
var_dump($storage->key());
var_dump($storage->current());

$storage->seek(3);
var_dump($storage->key());
var_dump($storage->current());

$storage->seek(3);
var_dump($storage->key());
var_dump($storage->current());

echo "--- With holes cases ---\n";

$storage->detach($b);
$storage->detach($d);

foreach (range(0, 2) as $index) {
    $storage->seek($index);
    var_dump($storage->key());
    var_dump($storage->current());
}

try {
    $storage->seek(3);
} catch (OutOfBoundsException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
--- Error cases ---
Seek position -1 is out of range
Seek position 5 is out of range
int(0)
object(Test)#1 (1) {
  ["marker"]=>
  string(1) "a"
}
--- Normal cases ---
int(2)
object(Test)#3 (1) {
  ["marker"]=>
  string(1) "c"
}
int(1)
object(Test)#2 (1) {
  ["marker"]=>
  string(1) "b"
}
int(4)
object(Test)#5 (1) {
  ["marker"]=>
  string(1) "e"
}
int(0)
object(Test)#1 (1) {
  ["marker"]=>
  string(1) "a"
}
int(3)
object(Test)#4 (1) {
  ["marker"]=>
  string(1) "d"
}
int(3)
object(Test)#4 (1) {
  ["marker"]=>
  string(1) "d"
}
--- With holes cases ---
int(0)
object(Test)#1 (1) {
  ["marker"]=>
  string(1) "a"
}
int(1)
object(Test)#3 (1) {
  ["marker"]=>
  string(1) "c"
}
int(2)
object(Test)#5 (1) {
  ["marker"]=>
  string(1) "e"
}
Seek position 3 is out of range
