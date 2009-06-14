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
unicode(10) "Original a"
unicode(10) "Original b"
unicode(10) "Original c"
object(C)#%d (5) {
  [u"a"]=>
  unicode(9) "changed.a"
  [u"b"]=>
  unicode(9) "changed.b"
  [u"c"]=>
  unicode(9) "changed.c"
  [u"d":protected]=>
  unicode(10) "Original d"
  [u"e":u"C":private]=>
  unicode(10) "Original e"
}

Modifying the foreach $value.
object(C)#%d (5) {
  [u"a"]=>
  unicode(10) "Original a"
  [u"b"]=>
  unicode(10) "Original b"
  [u"c"]=>
  unicode(10) "Original c"
  [u"d":protected]=>
  unicode(10) "Original d"
  [u"e":u"C":private]=>
  unicode(10) "Original e"
}

Modifying the foreach &$value.
object(C)#%d (5) {
  [u"a"]=>
  unicode(9) "changed.a"
  [u"b"]=>
  unicode(9) "changed.b"
  [u"c"]=>
  &unicode(9) "changed.c"
  [u"d":protected]=>
  unicode(10) "Original d"
  [u"e":u"C":private]=>
  unicode(10) "Original e"
}

Adding properties to an an object.
unicode(10) "Original a"
unicode(10) "Original b"
unicode(10) "Original c"
unicode(16) "Added property 0"
unicode(16) "Added property 1"
unicode(16) "Added property 2"
unicode(16) "Added property 3"
unicode(16) "Added property 4"
unicode(16) "Added property 5"
unicode(16) "Added property 6"
unicode(16) "Added property 7"
Loop detected
object(C)#%d (17) {
  [u"a"]=>
  unicode(10) "Original a"
  [u"b"]=>
  unicode(10) "Original b"
  [u"c"]=>
  unicode(10) "Original c"
  [u"d":protected]=>
  unicode(10) "Original d"
  [u"e":u"C":private]=>
  unicode(10) "Original e"
  [u"new0"]=>
  unicode(16) "Added property 0"
  [u"new1"]=>
  unicode(16) "Added property 1"
  [u"new2"]=>
  unicode(16) "Added property 2"
  [u"new3"]=>
  unicode(16) "Added property 3"
  [u"new4"]=>
  unicode(16) "Added property 4"
  [u"new5"]=>
  unicode(16) "Added property 5"
  [u"new6"]=>
  unicode(16) "Added property 6"
  [u"new7"]=>
  unicode(16) "Added property 7"
  [u"new8"]=>
  unicode(16) "Added property 8"
  [u"new9"]=>
  unicode(16) "Added property 9"
  [u"new10"]=>
  unicode(17) "Added property 10"
  [u"new11"]=>
  unicode(17) "Added property 11"
}

Adding properties to an an object, using &$value.
unicode(10) "Original a"
unicode(10) "Original b"
unicode(10) "Original c"
unicode(16) "Added property 0"
unicode(16) "Added property 1"
unicode(16) "Added property 2"
unicode(16) "Added property 3"
unicode(16) "Added property 4"
unicode(16) "Added property 5"
unicode(16) "Added property 6"
unicode(16) "Added property 7"
Loop detected
object(C)#%d (17) {
  [u"a"]=>
  unicode(10) "Original a"
  [u"b"]=>
  unicode(10) "Original b"
  [u"c"]=>
  unicode(10) "Original c"
  [u"d":protected]=>
  unicode(10) "Original d"
  [u"e":u"C":private]=>
  unicode(10) "Original e"
  [u"new0"]=>
  unicode(16) "Added property 0"
  [u"new1"]=>
  unicode(16) "Added property 1"
  [u"new2"]=>
  unicode(16) "Added property 2"
  [u"new3"]=>
  unicode(16) "Added property 3"
  [u"new4"]=>
  unicode(16) "Added property 4"
  [u"new5"]=>
  unicode(16) "Added property 5"
  [u"new6"]=>
  unicode(16) "Added property 6"
  [u"new7"]=>
  unicode(16) "Added property 7"
  [u"new8"]=>
  &unicode(16) "Added property 8"
  [u"new9"]=>
  unicode(16) "Added property 9"
  [u"new10"]=>
  unicode(17) "Added property 10"
  [u"new11"]=>
  unicode(17) "Added property 11"
}

Removing properties from an object.
unicode(10) "Original a"
object(C)#%d (2) {
  [u"d":protected]=>
  unicode(10) "Original d"
  [u"e":u"C":private]=>
  unicode(10) "Original e"
}

Removing properties from an object, using &$value.
unicode(10) "Original a"
object(C)#%d (2) {
  [u"d":protected]=>
  unicode(10) "Original d"
  [u"e":u"C":private]=>
  unicode(10) "Original e"
}
===DONE===