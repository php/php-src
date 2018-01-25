--TEST--
Foreach loop tests - modifying the object during the loop.
--FILE--
<?php

class C {
	public $a = "Original a";
	public $b = "Original b";
	public $c = "Original c";
	protected $d = "Original d";
	private $e = "Original e";
}

echo "\nDirectly changing object values.\n";
$obj = new C;
foreach ($obj as $k=>$v) {
	$obj->$k="changed.$k";
	var_dump($v);
}
var_dump($obj);

echo "\nModifying the foreach \$value.\n";
$obj = new C;
foreach ($obj as $k=>$v) {
	$v="changed.$k";
}
var_dump($obj);


echo "\nModifying the foreach &\$value.\n";
$obj = new C;
foreach ($obj as $k=>&$v) {
	$v="changed.$k";
}
var_dump($obj);

echo "\nAdding properties to an an object.\n";
$obj = new C;
$counter=0;
foreach ($obj as $v) {
	$newPropName = "new$counter";
	$obj->$newPropName = "Added property $counter";
    if ($counter++>10) {
    	echo "Loop detected\n";
    	break;
    }
	var_dump($v);
}
var_dump($obj);

echo "\nAdding properties to an an object, using &\$value.\n";
$obj = new C;
$counter=0;
foreach ($obj as &$v) {
	$newPropName = "new$counter";
	$obj->$newPropName = "Added property $counter";
    if ($counter++>10) {
    	echo "Loop detected\n";
    	break;
    }
	var_dump($v);
}
var_dump($obj);

echo "\nRemoving properties from an object.\n";
$obj = new C;
foreach ($obj as $v) {
	unset($obj->a);
	unset($obj->b);
	unset($obj->c);
	var_dump($v);
}
var_dump($obj);

echo "\nRemoving properties from an object, using &\$value.\n";
$obj = new C;
foreach ($obj as &$v) {
	unset($obj->a);
	unset($obj->b);
	unset($obj->c);
	var_dump($v);
}
var_dump($obj);

?>
===DONE===
--EXPECTF--
Directly changing object values.
string(10) "Original a"
string(10) "Original b"
string(10) "Original c"
object(C)#%d (5) {
  ["a"]=>
  string(9) "changed.a"
  ["b"]=>
  string(9) "changed.b"
  ["c"]=>
  string(9) "changed.c"
  ["d":protected]=>
  string(10) "Original d"
  ["e":"C":private]=>
  string(10) "Original e"
}

Modifying the foreach $value.
object(C)#%d (5) {
  ["a"]=>
  string(10) "Original a"
  ["b"]=>
  string(10) "Original b"
  ["c"]=>
  string(10) "Original c"
  ["d":protected]=>
  string(10) "Original d"
  ["e":"C":private]=>
  string(10) "Original e"
}

Modifying the foreach &$value.
object(C)#%d (5) {
  ["a"]=>
  string(9) "changed.a"
  ["b"]=>
  string(9) "changed.b"
  ["c"]=>
  &string(9) "changed.c"
  ["d":protected]=>
  string(10) "Original d"
  ["e":"C":private]=>
  string(10) "Original e"
}

Adding properties to an an object.
string(10) "Original a"
string(10) "Original b"
string(10) "Original c"
string(16) "Added property 0"
string(16) "Added property 1"
string(16) "Added property 2"
string(16) "Added property 3"
string(16) "Added property 4"
string(16) "Added property 5"
string(16) "Added property 6"
string(16) "Added property 7"
Loop detected
object(C)#%d (17) {
  ["a"]=>
  string(10) "Original a"
  ["b"]=>
  string(10) "Original b"
  ["c"]=>
  string(10) "Original c"
  ["d":protected]=>
  string(10) "Original d"
  ["e":"C":private]=>
  string(10) "Original e"
  ["new0"]=>
  string(16) "Added property 0"
  ["new1"]=>
  string(16) "Added property 1"
  ["new2"]=>
  string(16) "Added property 2"
  ["new3"]=>
  string(16) "Added property 3"
  ["new4"]=>
  string(16) "Added property 4"
  ["new5"]=>
  string(16) "Added property 5"
  ["new6"]=>
  string(16) "Added property 6"
  ["new7"]=>
  string(16) "Added property 7"
  ["new8"]=>
  string(16) "Added property 8"
  ["new9"]=>
  string(16) "Added property 9"
  ["new10"]=>
  string(17) "Added property 10"
  ["new11"]=>
  string(17) "Added property 11"
}

Adding properties to an an object, using &$value.
string(10) "Original a"
string(10) "Original b"
string(10) "Original c"
string(16) "Added property 0"
string(16) "Added property 1"
string(16) "Added property 2"
string(16) "Added property 3"
string(16) "Added property 4"
string(16) "Added property 5"
string(16) "Added property 6"
string(16) "Added property 7"
Loop detected
object(C)#%d (17) {
  ["a"]=>
  string(10) "Original a"
  ["b"]=>
  string(10) "Original b"
  ["c"]=>
  string(10) "Original c"
  ["d":protected]=>
  string(10) "Original d"
  ["e":"C":private]=>
  string(10) "Original e"
  ["new0"]=>
  string(16) "Added property 0"
  ["new1"]=>
  string(16) "Added property 1"
  ["new2"]=>
  string(16) "Added property 2"
  ["new3"]=>
  string(16) "Added property 3"
  ["new4"]=>
  string(16) "Added property 4"
  ["new5"]=>
  string(16) "Added property 5"
  ["new6"]=>
  string(16) "Added property 6"
  ["new7"]=>
  string(16) "Added property 7"
  ["new8"]=>
  &string(16) "Added property 8"
  ["new9"]=>
  string(16) "Added property 9"
  ["new10"]=>
  string(17) "Added property 10"
  ["new11"]=>
  string(17) "Added property 11"
}

Removing properties from an object.
string(10) "Original a"
object(C)#%d (2) {
  ["d":protected]=>
  string(10) "Original d"
  ["e":"C":private]=>
  string(10) "Original e"
}

Removing properties from an object, using &$value.
string(10) "Original a"
object(C)#%d (2) {
  ["d":protected]=>
  string(10) "Original d"
  ["e":"C":private]=>
  string(10) "Original e"
}
===DONE===
