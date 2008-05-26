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
Notice: Array to string conversion in %s on line %d
unicode(24) "Arraythis is test object"

Notice: Array to string conversion in %s on line %d
unicode(16) "Arraysome string"

Notice: Array to string conversion in %s on line %d
unicode(8) "Array222"

Notice: Array to string conversion in %s on line %d
unicode(13) "Array2323.444"

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d
unicode(10) "ArrayArray"

Notice: Array to string conversion in %s on line %d
unicode(24) "this is test objectArray"
unicode(30) "this is test objectsome string"
unicode(22) "this is test object222"
unicode(27) "this is test object2323.444"
unicode(38) "this is test objectthis is test object"
unicode(30) "some stringthis is test object"

Notice: Array to string conversion in %s on line %d
unicode(16) "some stringArray"
unicode(14) "some string222"
unicode(19) "some string2323.444"
unicode(22) "some stringsome string"

Notice: Array to string conversion in %s on line %d
unicode(8) "222Array"
unicode(22) "222this is test object"
unicode(14) "222some string"
unicode(11) "2222323.444"
unicode(6) "222222"

Notice: Array to string conversion in %s on line %d
unicode(13) "2323.444Array"
unicode(27) "2323.444this is test object"
unicode(19) "2323.444some string"
unicode(11) "2323.444222"
unicode(16) "2323.4442323.444"
Done
