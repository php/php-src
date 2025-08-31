--TEST--
SimpleXML: implement Countable
--EXTENSIONS--
simplexml
--FILE--
<?php

$str = '<xml></xml>';
$sxe = new SimpleXmlElement($str);
var_dump($sxe instanceof Countable);
?>
--EXPECT--
bool(true)
