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
string(10) "Original a"
string(10) "Original b"
string(10) "Original c"
string(1) "a"
string(10) "Original a"
string(1) "b"
string(10) "Original b"
string(1) "c"
string(10) "Original c"

Check key and value after the loop.
string(1) "c"
string(10) "Original c"


Object instantiated inside loop.
string(10) "Original a"
string(10) "Original b"
string(10) "Original c"
string(1) "a"
string(10) "Original a"
string(1) "b"
string(10) "Original b"
string(1) "c"
string(10) "Original c"

Check key and value after the loop.
string(1) "c"
string(10) "Original c"
