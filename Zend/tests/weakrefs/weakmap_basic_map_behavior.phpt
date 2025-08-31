--TEST--
Basic WeakMap behavior (as a map)
--FILE--
<?php

$map = new WeakMap;
var_dump(count($map));

$obj = new stdClass;
$obj->value = 1;
$obj2 = new stdClass;
$obj2->value = 2;

$map[$obj] = $obj2;
var_dump(count($map));
var_dump($map);
var_dump(isset($map[$obj]));
var_dump(!empty($map[$obj]));
var_dump($map[$obj]);

$map[$obj] = 42;
var_dump($map);
var_dump(isset($map[$obj]));
var_dump(!empty($map[$obj]));
var_dump($map[$obj]);

$map[$obj] = false;
var_dump($map);
var_dump(isset($map[$obj]));
var_dump(!empty($map[$obj]));
var_dump($map[$obj]);

$map[$obj] = null;
var_dump($map);
var_dump(isset($map[$obj]));
var_dump(!empty($map[$obj]));
var_dump($map[$obj]);

unset($map[$obj]);
var_dump($map);
var_dump(isset($map[$obj]));
var_dump(!empty($map[$obj]));
try {
    var_dump($map[$obj]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

// It's okay to unset an object that's not in the map.
unset($map[new stdClass]);

echo "\nIndirect modification:\n";
$map[$obj] = [];
$map[$obj][] = 42;
$map[$obj2] = 41;
$map[$obj2]++;
var_dump($map);

echo "\nMethods:\n";
var_dump($map->offsetSet($obj2, 43));
var_dump($map->offsetGet($obj2));
var_dump($map->offsetExists($obj2));
var_dump($map->count());
var_dump($map->offsetUnset($obj2));
var_dump($map->count());

?>
--EXPECT--
int(0)
int(1)
object(WeakMap)#1 (1) {
  [0]=>
  array(2) {
    ["key"]=>
    object(stdClass)#2 (1) {
      ["value"]=>
      int(1)
    }
    ["value"]=>
    object(stdClass)#3 (1) {
      ["value"]=>
      int(2)
    }
  }
}
bool(true)
bool(true)
object(stdClass)#3 (1) {
  ["value"]=>
  int(2)
}
object(WeakMap)#1 (1) {
  [0]=>
  array(2) {
    ["key"]=>
    object(stdClass)#2 (1) {
      ["value"]=>
      int(1)
    }
    ["value"]=>
    int(42)
  }
}
bool(true)
bool(true)
int(42)
object(WeakMap)#1 (1) {
  [0]=>
  array(2) {
    ["key"]=>
    object(stdClass)#2 (1) {
      ["value"]=>
      int(1)
    }
    ["value"]=>
    bool(false)
  }
}
bool(true)
bool(false)
bool(false)
object(WeakMap)#1 (1) {
  [0]=>
  array(2) {
    ["key"]=>
    object(stdClass)#2 (1) {
      ["value"]=>
      int(1)
    }
    ["value"]=>
    NULL
  }
}
bool(false)
bool(false)
NULL
object(WeakMap)#1 (0) {
}
bool(false)
bool(false)
Object stdClass#2 not contained in WeakMap

Indirect modification:
object(WeakMap)#1 (2) {
  [0]=>
  array(2) {
    ["key"]=>
    object(stdClass)#2 (1) {
      ["value"]=>
      int(1)
    }
    ["value"]=>
    array(1) {
      [0]=>
      int(42)
    }
  }
  [1]=>
  array(2) {
    ["key"]=>
    object(stdClass)#3 (1) {
      ["value"]=>
      int(2)
    }
    ["value"]=>
    int(42)
  }
}

Methods:
NULL
int(43)
bool(true)
int(2)
NULL
int(1)
