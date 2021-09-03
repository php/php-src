--TEST--
SPL: MultipleIterator
--FILE--
<?php

$iter1 = new ArrayIterator(array(1,2,3));
$iter2 = new ArrayIterator(array(1,2));
$iter3 = new ArrayIterator(array(new stdClass(),"string",3));

$m = new MultipleIterator();

echo "-- Default flags, no iterators --\n";
foreach($m as $value) {
    var_dump($value);
}
try {
    var_dump($m->current());
} catch (RuntimeException $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($m->key());
} catch (RuntimeException $e) {
    echo $e->getMessage(), "\n";
}

$m->attachIterator($iter1);
$m->attachIterator($iter2);
$m->attachIterator($iter3);

echo "-- Default flags, MultipleIterator::MIT_NEED_ALL | MultipleIterator::MIT_KEYS_NUMERIC --\n";

var_dump($m->getFlags() === (MultipleIterator::MIT_NEED_ALL | MultipleIterator::MIT_KEYS_NUMERIC));

foreach($m as $key => $value) {
    var_dump($key, $value);
}
try {
    $m->current();
} catch(RuntimeException $e) {
    echo "RuntimeException thrown: " . $e->getMessage() . "\n";
}
try {
    $m->key();
} catch(RuntimeException $e) {
    echo "RuntimeException thrown: " . $e->getMessage() . "\n";
}

echo "-- Flags = MultipleIterator::MIT_NEED_ANY | MultipleIterator::MIT_KEYS_NUMERIC --\n";

$m->setFlags(MultipleIterator::MIT_NEED_ANY | MultipleIterator::MIT_KEYS_NUMERIC);
var_dump($m->getFlags() === (MultipleIterator::MIT_NEED_ANY | MultipleIterator::MIT_KEYS_NUMERIC));

foreach($m as $key => $value) {
    var_dump($key, $value);
}

echo "-- Default flags, added element --\n";

$m->setFlags(MultipleIterator::MIT_NEED_ALL | MultipleIterator::MIT_KEYS_NUMERIC);

$iter2[] = 3;
foreach($m as $key => $value) {
    var_dump($key, $value);
}

echo "-- Flags |= MultipleIterator::MIT_KEYS_ASSOC, with iterator associated with NULL --\n";

$m->setFlags(MultipleIterator::MIT_NEED_ALL | MultipleIterator::MIT_KEYS_ASSOC);
$m->rewind();
try {
    $m->current();
} catch(InvalidArgumentException $e) {
    echo "InvalidArgumentException thrown: " . $e->getMessage() . "\n";
}

echo "-- Flags |= MultipleIterator::MIT_KEYS_ASSOC --\n";

$m->attachIterator($iter1, "iter1");
$m->attachIterator($iter2, "iter2");
$m->attachIterator($iter3, 3);

foreach($m as $key => $value) {
    var_dump($key, $value);
}

echo "-- Associate with invalid value --\n";

try {
    $m->attachIterator($iter3, new stdClass());
} catch(TypeError $e) {
    echo "TypeError thrown: " . $e->getMessage() . "\n";
}

echo "-- Associate with duplicate value --\n";

try {
    $m->attachIterator($iter3, "iter1");
} catch(InvalidArgumentException $e) {
    echo "InvalidArgumentException thrown: " . $e->getMessage() . "\n";
}

echo "-- Count, contains, detach, count, contains, iterate --\n";

var_dump($m->countIterators());
var_dump($m->containsIterator($iter2));
var_dump($m->detachIterator($iter2));
var_dump($m->countIterators());
var_dump($m->containsIterator($iter2));
foreach($m as $key => $value) {
    var_dump($key, $value);
}

?>
--EXPECTF--
-- Default flags, no iterators --
Called current() on an invalid iterator
Called key() on an invalid iterator
-- Default flags, MultipleIterator::MIT_NEED_ALL | MultipleIterator::MIT_KEYS_NUMERIC --
bool(true)
array(3) {
  [0]=>
  int(0)
  [1]=>
  int(0)
  [2]=>
  int(0)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(1)
  [2]=>
  object(stdClass)#%d (0) {
  }
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(1)
  [2]=>
  int(1)
}
array(3) {
  [0]=>
  int(2)
  [1]=>
  int(2)
  [2]=>
  string(6) "string"
}
RuntimeException thrown: Called current() with non valid sub iterator
RuntimeException thrown: Called key() with non valid sub iterator
-- Flags = MultipleIterator::MIT_NEED_ANY | MultipleIterator::MIT_KEYS_NUMERIC --
bool(true)
array(3) {
  [0]=>
  int(0)
  [1]=>
  int(0)
  [2]=>
  int(0)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(1)
  [2]=>
  object(stdClass)#%d (0) {
  }
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(1)
  [2]=>
  int(1)
}
array(3) {
  [0]=>
  int(2)
  [1]=>
  int(2)
  [2]=>
  string(6) "string"
}
array(3) {
  [0]=>
  int(2)
  [1]=>
  NULL
  [2]=>
  int(2)
}
array(3) {
  [0]=>
  int(3)
  [1]=>
  NULL
  [2]=>
  int(3)
}
-- Default flags, added element --
array(3) {
  [0]=>
  int(0)
  [1]=>
  int(0)
  [2]=>
  int(0)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(1)
  [2]=>
  object(stdClass)#%d (0) {
  }
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(1)
  [2]=>
  int(1)
}
array(3) {
  [0]=>
  int(2)
  [1]=>
  int(2)
  [2]=>
  string(6) "string"
}
array(3) {
  [0]=>
  int(2)
  [1]=>
  int(2)
  [2]=>
  int(2)
}
array(3) {
  [0]=>
  int(3)
  [1]=>
  int(3)
  [2]=>
  int(3)
}
-- Flags |= MultipleIterator::MIT_KEYS_ASSOC, with iterator associated with NULL --
InvalidArgumentException thrown: Sub-Iterator is associated with NULL
-- Flags |= MultipleIterator::MIT_KEYS_ASSOC --
array(3) {
  ["iter1"]=>
  int(0)
  ["iter2"]=>
  int(0)
  [3]=>
  int(0)
}
array(3) {
  ["iter1"]=>
  int(1)
  ["iter2"]=>
  int(1)
  [3]=>
  object(stdClass)#%d (0) {
  }
}
array(3) {
  ["iter1"]=>
  int(1)
  ["iter2"]=>
  int(1)
  [3]=>
  int(1)
}
array(3) {
  ["iter1"]=>
  int(2)
  ["iter2"]=>
  int(2)
  [3]=>
  string(6) "string"
}
array(3) {
  ["iter1"]=>
  int(2)
  ["iter2"]=>
  int(2)
  [3]=>
  int(2)
}
array(3) {
  ["iter1"]=>
  int(3)
  ["iter2"]=>
  int(3)
  [3]=>
  int(3)
}
-- Associate with invalid value --
TypeError thrown: MultipleIterator::attachIterator(): Argument #2 ($info) must be of type string|int|null, stdClass given
-- Associate with duplicate value --
InvalidArgumentException thrown: Key duplication error
-- Count, contains, detach, count, contains, iterate --
int(3)
bool(true)
NULL
int(2)
bool(false)
array(2) {
  ["iter1"]=>
  int(0)
  [3]=>
  int(0)
}
array(2) {
  ["iter1"]=>
  int(1)
  [3]=>
  object(stdClass)#%d (0) {
  }
}
array(2) {
  ["iter1"]=>
  int(1)
  [3]=>
  int(1)
}
array(2) {
  ["iter1"]=>
  int(2)
  [3]=>
  string(6) "string"
}
array(2) {
  ["iter1"]=>
  int(2)
  [3]=>
  int(2)
}
array(2) {
  ["iter1"]=>
  int(3)
  [3]=>
  int(3)
}
