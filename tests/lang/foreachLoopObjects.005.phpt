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
===DONE===
--EXPECTF--
Removing properties before the current element from an iterated object.
unicode(10) "Original a"
unicode(10) "Original b"
unicode(10) "Original d"
unicode(10) "Original e"
object(C)#%d (4) {
  [u"a"]=>
  unicode(10) "Original a"
  [u"b"]=>
  unicode(10) "Original b"
  [u"d"]=>
  unicode(10) "Original d"
  [u"e"]=>
  unicode(10) "Original e"
}

Removing properties before the current element from an iterated object.
unicode(10) "Original a"
unicode(10) "Original b"
unicode(10) "Original c"
unicode(10) "Original d"
unicode(10) "Original e"
object(C)#%d (4) {
  [u"b"]=>
  unicode(10) "Original b"
  [u"c"]=>
  unicode(10) "Original c"
  [u"d"]=>
  unicode(10) "Original d"
  [u"e"]=>
  unicode(10) "Original e"
}
===DONE===