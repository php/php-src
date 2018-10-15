--TEST--
simplexml_load_file()
--SKIPIF--
<?php if (!extension_loaded("simplexml")) die("skip"); ?>
--FILE--
<?php
	$simple = simplexml_load_file(dirname(__FILE__)."/book.xml");

	var_dump($simple);
	echo "Done";
?>
--EXPECT--
object(SimpleXMLElement)#1 (1) {
  ["book"]=>
  array(2) {
    [0]=>
    object(SimpleXMLElement)#2 (2) {
      ["title"]=>
      string(19) "The Grapes of Wrath"
      ["author"]=>
      string(14) "John Steinbeck"
    }
    [1]=>
    object(SimpleXMLElement)#3 (2) {
      ["title"]=>
      string(9) "The Pearl"
      ["author"]=>
      string(14) "John Steinbeck"
    }
  }
}
Done
