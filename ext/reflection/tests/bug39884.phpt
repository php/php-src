--TEST--
Bug #39884 (ReflectionParameter::getClass() throws exception for type hint self)
--SKIPIF--
<?php extension_loaded('reflection') or die('skip'); ?>
--FILE--
<?php
class stubParamTest
{
    function paramTest(self $param)
    {
        // nothing to do
    }
}
$test1 = new stubParamTest();
$test2 = new stubParamTest();
$test1->paramTest($test2);
$refParam = new ReflectionParameter(array('stubParamTest', 'paramTest'), 'param');
var_dump($refParam->getClass());
?>
--EXPECT--	
object(ReflectionClass)#4 (1) {
  ["name"]=>
  string(13) "stubParamTest"
}
