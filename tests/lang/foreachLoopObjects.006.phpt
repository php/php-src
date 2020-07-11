--TEST--
Foreach loop tests - substituting the entire iterated entity during the loop.
--FILE--
<?php

class C {
    public $a = "Original a";
    public $b = "Original b";
    public $c = "Original c";
    public $d = "Original d";
    public $e = "Original e";
}

echo "\nSubstituting the iterated object for a different object.\n";
$obj = new C;
$obj2 = new stdclass;
$obj2->a = "new a";
$obj2->b = "new b";
$obj2->c = "new c";
$obj2->d = "new d";
$obj2->e = "new e";
$obj2->f = "new f";
$ref = &$obj;
$count=0;
foreach ($obj as $v) {
    var_dump($v);
    if ($v==$obj->b) {
      $ref=$obj2;
    }
    if (++$count>10) {
        echo "Loop detected.\n";
        break;
    }
}
var_dump($obj);

echo "\nSubstituting the iterated object for an array.\n";
$obj = new C;
$a = array(1,2,3,4,5,6,7,8);
$ref = &$obj;
$count=0;
foreach ($obj as $v) {
    var_dump($v);
    if ($v==="Original b") {
      $ref=$a;
    }
    if (++$count>10) {
        echo "Loop detected.\n";
        break;
    }
}
var_dump($obj);

echo "\nSubstituting the iterated array for an object.\n";
$a = array(1,2,3,4,5,6,7,8);
$obj = new C;
$ref = &$a;
$count=0;
foreach ($a as $v) {
    var_dump($v);
    if ($v===2) {
      $ref=$obj;
    }
    if (++$count>10) {
        echo "Loop detected.\n";
        break;
    }
}
var_dump($obj);

?>
--EXPECTF--
Substituting the iterated object for a different object.
string(10) "Original a"
string(10) "Original b"
string(10) "Original c"
string(10) "Original d"
string(10) "Original e"
object(stdClass)#%d (6) {
  ["a"]=>
  string(5) "new a"
  ["b"]=>
  string(5) "new b"
  ["c"]=>
  string(5) "new c"
  ["d"]=>
  string(5) "new d"
  ["e"]=>
  string(5) "new e"
  ["f"]=>
  string(5) "new f"
}

Substituting the iterated object for an array.
string(10) "Original a"
string(10) "Original b"
string(10) "Original c"
string(10) "Original d"
string(10) "Original e"
array(8) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  int(5)
  [5]=>
  int(6)
  [6]=>
  int(7)
  [7]=>
  int(8)
}

Substituting the iterated array for an object.
int(1)
int(2)
int(3)
int(4)
int(5)
int(6)
int(7)
int(8)
object(C)#%d (5) {
  ["a"]=>
  string(10) "Original a"
  ["b"]=>
  string(10) "Original b"
  ["c"]=>
  string(10) "Original c"
  ["d"]=>
  string(10) "Original d"
  ["e"]=>
  string(10) "Original e"
}
