--TEST--
SimpleXML and foreach 
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php 
$sxe = simplexml_load_file(dirname(__FILE__).'/009.xml');
foreach($sxe as $name=>$val) {
	var_dump($name);
	var_dump(get_class($val));
	var_dump(trim((string)$val));
}
?>
===DONE===
--EXPECT--
string(5) "elem1"
string(17) "simplexml_element"
string(10) "Bla bla 1."
string(6) "elem11"
string(17) "simplexml_element"
string(10) "Bla bla 2."
===DONE===
