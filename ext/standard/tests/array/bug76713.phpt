--TEST--
Bug #76713 (Segmentation fault caused by property corruption)
--FILE--
<?php

function test($obj) {
	return array_column(array($obj), "prop");
}

$obj = new Stdclass();

$obj->prop = str_pad("a", 10, 'a');

test($obj);
test($obj);
test($obj);

var_dump($obj->prop);

class C {
	public $name;
	public function __get($name) {
		return $this->name;
	}
}

$obj = new C;

$obj->name = str_pad("b", 10, 'b');

test($obj);
test($obj);
test($obj);

var_dump($obj->prop);
?>
--EXPECT--
string(10) "aaaaaaaaaa"
string(10) "bbbbbbbbbb"
