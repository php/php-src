--TEST--
Foreach loop tests - Removing the current element from an iterated object.
--FILE--
<?php

class C {
    public $a = "Original a";
    public $b = "Original b";
    public $c = "Original c";
    public $d = "Original d";
    public $e = "Original e";
}

echo "\nRemoving the current element from an iterated object.\n";
$obj = new C;
$count=0;
foreach ($obj as $v) {
    if ($v==$obj->b) {
        unset($obj->b);
    }
    var_dump($v);
    if (++$count>10) {
        echo "Loop detected.\n";
        break;
    }
}
var_dump($obj);


?>
--EXPECTF--
Removing the current element from an iterated object.
string(10) "Original a"
string(10) "Original b"

Warning: Undefined property: C::$b in %s on line %d
string(10) "Original c"

Warning: Undefined property: C::$b in %s on line %d
string(10) "Original d"

Warning: Undefined property: C::$b in %s on line %d
string(10) "Original e"
object(C)#%d (4) {
  ["a"]=>
  string(10) "Original a"
  ["c"]=>
  string(10) "Original c"
  ["d"]=>
  string(10) "Original d"
  ["e"]=>
  string(10) "Original e"
}
