--TEST--
XSLTProcessor: new instance without constructor
--EXTENSIONS--
xsl
--FILE--
<?php

$rc = new ReflectionClass('XSLTProcessor');
$processor = $rc->newInstanceWithoutConstructor();
var_dump($processor);

?>
--EXPECTF--
object(XSLTProcessor)#2 (4) {
  ["doXInclude"]=>
  bool(false)
  ["cloneDocument"]=>
  bool(false)
  ["maxTemplateDepth"]=>
  int(%d)
  ["maxTemplateVars"]=>
  int(%d)
}
