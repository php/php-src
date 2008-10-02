--TEST--
SimpleXMLElement->addAttribute()
--SKIPIF-- 
<?php if (!extension_loaded("simplexml")) die("skip"); ?>
--FILE-- 
<?php
	$simple = simplexml_load_file(dirname(__FILE__)."/book.xml");
	$simple->addAttribute('type','novels');
	
	var_dump($simple->attributes());
	echo "Done";
?>
--EXPECTF--
object(SimpleXMLElement)#2 (1) {
  [u"@attributes"]=>
  array(1) {
    [u"type"]=>
    unicode(6) "novels"
  }
}
Done
