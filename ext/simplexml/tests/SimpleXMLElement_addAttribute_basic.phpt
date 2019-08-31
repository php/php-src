--TEST--
SimpleXMLElement->addAttribute()
--SKIPIF--
<?php if (!extension_loaded("simplexml")) die("skip"); ?>
--FILE--
<?php
	$simple = simplexml_load_file(__DIR__."/book.xml");
	$simple->addAttribute('type','novels');

	var_dump($simple->attributes());
	echo "Done";
?>
--EXPECT--
object(SimpleXMLElement)#2 (1) {
  ["@attributes"]=>
  array(1) {
    ["type"]=>
    string(6) "novels"
  }
}
Done
