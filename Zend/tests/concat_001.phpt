--TEST--
concat difffent types
--INI--
precision=14
--FILE--
<?php

class test {
	function __toString() {
		return "this is test object";
	}
}

$a = array(1,2,3);
$o = new test;
$s = "some string";
$i = 222;
$d = 2323.444;

var_dump($a.$o);
var_dump($a.$s);
var_dump($a.$i);
var_dump($a.$d);
var_dump($a.$a);

var_dump($o.$a);
var_dump($o.$s);
var_dump($o.$i);
var_dump($o.$d);
var_dump($o.$o);

var_dump($s.$o);
var_dump($s.$a);
var_dump($s.$i);
var_dump($s.$d);
var_dump($s.$s);

var_dump($i.$a);
var_dump($i.$o);
var_dump($i.$s);
var_dump($i.$d);
var_dump($i.$i);

var_dump($d.$a);
var_dump($d.$o);
var_dump($d.$s);
var_dump($d.$i);
var_dump($d.$d);

echo "Done\n";
?>
--EXPECTF--	
string(24) "Arraythis is test object"
string(16) "Arraysome string"
string(8) "Array222"
string(13) "Array2323.444"
string(10) "ArrayArray"
string(24) "this is test objectArray"
string(30) "this is test objectsome string"
string(22) "this is test object222"
string(27) "this is test object2323.444"
string(38) "this is test objectthis is test object"
string(30) "some stringthis is test object"
string(16) "some stringArray"
string(14) "some string222"
string(19) "some string2323.444"
string(22) "some stringsome string"
string(8) "222Array"
string(22) "222this is test object"
string(14) "222some string"
string(11) "2222323.444"
string(6) "222222"
string(13) "2323.444Array"
string(27) "2323.444this is test object"
string(19) "2323.444some string"
string(11) "2323.444222"
string(16) "2323.4442323.444"
Done
