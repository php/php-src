--TEST--
SimpleXML and foreach
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php 

$sxe = simplexml_load_file(dirname(__FILE__).'/006.xml');

foreach($sxe as $name => $data) {
	var_dump($name);
	var_dump(trim($data));
}

foreach($sxe->__clone() as $name => $data) {
	var_dump($name);
	var_dump(trim($data));
}

foreach($sxe->elem1 as $name => $data) {
	var_dump($name);
	var_dump(trim($data));
}

echo "===Done===\n";

?>
--EXPECT--
string(5) "elem1"
string(10) "Bla bla 1."
string(6) "elem11"
string(10) "Bla bla 2."
string(5) "elem1"
string(10) "Bla bla 1."
string(6) "elem11"
string(10) "Bla bla 2."
string(7) "comment"
string(0) ""
string(5) "elem2"
string(28) "Here we have some text data."
===Done===
