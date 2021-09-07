--TEST--
Object serialization / unserialization: real references and COW references
--FILE--
<?php
echo "\n\nArray containing same object twice:\n";
$obj = new stdclass;
$a[0] = $obj;
$a[1] = $a[0];
var_dump($a);

$ser = serialize($a);
var_dump($ser);

$ua = unserialize($ser);
var_dump($ua);
$ua[0]->a = "newProp";
var_dump($ua);
$ua[0] = "a0.changed";
var_dump($ua);


echo "\n\nArray containing object and reference to that object:\n";
$obj = new stdclass;
$a[0] = $obj;
$a[1] = &$a[0];
var_dump($a);

$ser = serialize($a);
var_dump($ser);

$ua = unserialize($ser);
var_dump($ua);
$ua[0]->a = "newProp";
var_dump($ua);
$ua[0] = "a0.changed";
var_dump($ua);

echo "\n\nObject containing same object twice:";
$obj = new stdclass;
$contaner = new stdclass;
$contaner->a = $obj;
$contaner->b = $contaner->a;
var_dump($contaner);

$ser = serialize($contaner);
var_dump($ser);

$ucontainer = unserialize($ser);
var_dump($ucontainer);
$ucontainer->a->a = "newProp";
var_dump($ucontainer);
$ucontainer->a = "container->a.changed";
var_dump($ucontainer);


echo "\n\nObject containing object and reference to that object:\n";
$obj = new stdclass;
$contaner = new stdclass;
$contaner->a = $obj;
$contaner->b = &$contaner->a;
var_dump($contaner);

$ser = serialize($contaner);
var_dump($ser);

$ucontainer = unserialize($ser);
var_dump($ucontainer);
$ucontainer->a->a = "newProp";
var_dump($ucontainer);
$ucontainer->b = "container->a.changed";
var_dump($ucontainer);

echo "Done";
?>
--EXPECT--
Array containing same object twice:
array(2) {
  [0]=>
  object(DynamicObject)#1 (0) {
  }
  [1]=>
  object(DynamicObject)#1 (0) {
  }
}
string(43) "a:2:{i:0;O:13:"DynamicObject":0:{}i:1;r:2;}"
array(2) {
  [0]=>
  object(DynamicObject)#2 (0) {
  }
  [1]=>
  object(DynamicObject)#2 (0) {
  }
}
array(2) {
  [0]=>
  object(DynamicObject)#2 (1) {
    ["a"]=>
    string(7) "newProp"
  }
  [1]=>
  object(DynamicObject)#2 (1) {
    ["a"]=>
    string(7) "newProp"
  }
}
array(2) {
  [0]=>
  string(10) "a0.changed"
  [1]=>
  object(DynamicObject)#2 (1) {
    ["a"]=>
    string(7) "newProp"
  }
}


Array containing object and reference to that object:
array(2) {
  [0]=>
  &object(DynamicObject)#3 (0) {
  }
  [1]=>
  &object(DynamicObject)#3 (0) {
  }
}
string(43) "a:2:{i:0;O:13:"DynamicObject":0:{}i:1;R:2;}"
array(2) {
  [0]=>
  &object(DynamicObject)#1 (0) {
  }
  [1]=>
  &object(DynamicObject)#1 (0) {
  }
}
array(2) {
  [0]=>
  &object(DynamicObject)#1 (1) {
    ["a"]=>
    string(7) "newProp"
  }
  [1]=>
  &object(DynamicObject)#1 (1) {
    ["a"]=>
    string(7) "newProp"
  }
}
array(2) {
  [0]=>
  &string(10) "a0.changed"
  [1]=>
  &string(10) "a0.changed"
}


Object containing same object twice:object(DynamicObject)#2 (2) {
  ["a"]=>
  object(DynamicObject)#1 (0) {
  }
  ["b"]=>
  object(DynamicObject)#1 (0) {
  }
}
string(70) "O:13:"DynamicObject":2:{s:1:"a";O:13:"DynamicObject":0:{}s:1:"b";r:2;}"
object(DynamicObject)#4 (2) {
  ["a"]=>
  object(DynamicObject)#5 (0) {
  }
  ["b"]=>
  object(DynamicObject)#5 (0) {
  }
}
object(DynamicObject)#4 (2) {
  ["a"]=>
  object(DynamicObject)#5 (1) {
    ["a"]=>
    string(7) "newProp"
  }
  ["b"]=>
  object(DynamicObject)#5 (1) {
    ["a"]=>
    string(7) "newProp"
  }
}
object(DynamicObject)#4 (2) {
  ["a"]=>
  string(20) "container->a.changed"
  ["b"]=>
  object(DynamicObject)#5 (1) {
    ["a"]=>
    string(7) "newProp"
  }
}


Object containing object and reference to that object:
object(DynamicObject)#7 (2) {
  ["a"]=>
  &object(DynamicObject)#6 (0) {
  }
  ["b"]=>
  &object(DynamicObject)#6 (0) {
  }
}
string(70) "O:13:"DynamicObject":2:{s:1:"a";O:13:"DynamicObject":0:{}s:1:"b";R:2;}"
object(DynamicObject)#2 (2) {
  ["a"]=>
  &object(DynamicObject)#1 (0) {
  }
  ["b"]=>
  &object(DynamicObject)#1 (0) {
  }
}
object(DynamicObject)#2 (2) {
  ["a"]=>
  &object(DynamicObject)#1 (1) {
    ["a"]=>
    string(7) "newProp"
  }
  ["b"]=>
  &object(DynamicObject)#1 (1) {
    ["a"]=>
    string(7) "newProp"
  }
}
object(DynamicObject)#2 (2) {
  ["a"]=>
  &string(20) "container->a.changed"
  ["b"]=>
  &string(20) "container->a.changed"
}
Done
