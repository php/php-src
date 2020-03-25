--TEST--
Bug #74416 Wrong reflection on DOMNode::cloneNode
--SKIPIF--
<?php
require_once('skipif.inc');
?>
--FILE--
<?php
$rm = new ReflectionMethod(DOMNode::class, "cloneNode");
printf("%d\n%d\n", $rm->getNumberOfParameters(), $rm->getNumberOfRequiredParameters());
foreach ($rm->getParameters() as $param) {
    printf("Parameter #%d %s OPTIONAL\n", $param->getPosition(), $param->isOptional() ? "IS" : "IS NOT");
}
?>
--EXPECT--
1
0
Parameter #0 IS OPTIONAL
