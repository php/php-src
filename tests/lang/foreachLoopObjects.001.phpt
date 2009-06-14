--TEST--
Foreach loop on objects - basic loop with just value and key => value.
--FILE--
<?php

class C {
	public $a = "Original a";
	public $b = "Original b";
	public $c = "Original c";
	protected $d = "Original d";
	private $e = "Original e";
	
}

echo "\n\nSimple loop.\n";
$obj = new C;
foreach ($obj as $v) {
	var_dump($v);
}
foreach ($obj as $k => $v) {
	var_dump($k, $v);
}
echo "\nCheck key and value after the loop.\n";
var_dump($k, $v);


echo "\n\nObject instantiated inside loop.\n";
foreach (new C as $v) {
	var_dump($v);
}
foreach (new C as $k => $v) {
	var_dump($k, $v);
}
echo "\nCheck key and value after the loop.\n";
var_dump($k, $v);
?>
--EXPECTF--


Simple loop.
unicode(10) "Original a"
unicode(10) "Original b"
unicode(10) "Original c"
unicode(1) "a"
unicode(10) "Original a"
unicode(1) "b"
unicode(10) "Original b"
unicode(1) "c"
unicode(10) "Original c"

Check key and value after the loop.
unicode(1) "c"
unicode(10) "Original c"


Object instantiated inside loop.
unicode(10) "Original a"
unicode(10) "Original b"
unicode(10) "Original c"
unicode(1) "a"
unicode(10) "Original a"
unicode(1) "b"
unicode(10) "Original b"
unicode(1) "c"
unicode(10) "Original c"

Check key and value after the loop.
unicode(1) "c"
unicode(10) "Original c"