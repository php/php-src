--TEST--
SimpleXML and recursion
--SKIPIF--
<?php 
	if (!extension_loaded('simplexml')) print 'skip';
	if (!class_exists('RecursiveIteratorIterator')) print 'skip RecursiveIteratorIterator not available';
?>
--FILE--
<?php 

$sxe = simplexml_load_file(dirname(__FILE__).'/006.xml');

foreach(new RecursiveIteratorIterator($sxe, 1) as $name => $data) {
	var_dump($name);
	var_dump(get_class($data));
	var_dump(trim($data));
}

echo "===DUMP===\n";

var_dump(get_class($sxe));
var_dump(trim($sxe->elem1));

?>
===DONE===
--EXPECT--
string(5) "elem1"
string(17) "simplexml_element"
string(10) "Bla bla 1."
string(7) "comment"
string(17) "simplexml_element"
string(0) ""
string(5) "elem2"
string(17) "simplexml_element"
string(28) "Here we have some text data."
string(5) "elem3"
string(17) "simplexml_element"
string(19) "And here some more."
string(5) "elem4"
string(17) "simplexml_element"
string(15) "Wow once again."
string(6) "elem11"
string(17) "simplexml_element"
string(10) "Bla bla 2."
string(7) "elem111"
string(17) "simplexml_element"
string(7) "Foo Bar"
===DUMP===
string(17) "simplexml_element"
string(10) "Bla bla 1."
===DONE===
