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
--EXPECTF--
object(SimpleXMLElement)#1 (1) {
  [u"book"]=>
  array(2) {
    [0]=>
    object(SimpleXMLElement)#2 (2) {
      [u"title"]=>
      unicode(19) "The Grapes of Wrath"
      [u"author"]=>
      unicode(14) "John Steinbeck"
    }
    [1]=>
    object(SimpleXMLElement)#3 (2) {
      [u"title"]=>
      unicode(9) "The Pearl"
      [u"author"]=>
      unicode(14) "John Steinbeck"
    }
  }
}
Done
