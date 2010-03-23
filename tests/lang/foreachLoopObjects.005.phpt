--TEST--
Foreach loop tests - removing properties before and after the current property during the loop.
--FILE--
<?php

class C {
	public $a = "Original a";
	public $b = "Original b";
	public $c = "Original c";
	public $d = "Original d";
	public $e = "Original e";
}

echo "\nRemoving properties before the current element from an iterated object.\n";
$obj = new C;
$count=0;
foreach ($obj as $v) {
	if ($v==$obj->a) {
		unset($obj->c);	
	}
	var_dump($v);
	if (++$count>10) {
		echo "Loop detected.\n";
		break;
	}		
}
var_dump($obj);

echo "\nRemoving properties before the current element from an iterated object.\n";
$obj = new C;
foreach ($obj as $v) {
	if ($v==$obj->b) {
		unset($obj->a);	
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

Removing properties before the current element from an iterated object.
string(10) "Original a"
string(10) "Original b"
string(10) "Original d"
string(10) "Original e"
object(C)#%d (4) {
  ["a"]=>
  string(10) "Original a"
  ["b"]=>
  string(10) "Original b"
  ["d"]=>
  string(10) "Original d"
  ["e"]=>
  string(10) "Original e"
}

Removing properties before the current element from an iterated object.
string(10) "Original a"
string(10) "Original b"
string(10) "Original c"
string(10) "Original d"
string(10) "Original e"
object(C)#%d (4) {
  ["b"]=>
  string(10) "Original b"
  ["c"]=>
  string(10) "Original c"
  ["d"]=>
  string(10) "Original d"
  ["e"]=>
  string(10) "Original e"
}
