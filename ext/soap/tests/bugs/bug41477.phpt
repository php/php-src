--TEST--
Bug #41477 (no arginfo about SoapClient::__soapCall())
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$objRfClass = new ReflectionClass('SoapClient');
$objRfMethod = $objRfClass->getMethod('__soapCall');
$arrParams = $objRfMethod->getParameters();
foreach($arrParams as $objRfParam)
{
        var_dump($objRfParam->getName());
}
?>
--EXPECT--
string(13) "function_name"
string(9) "arguments"
string(7) "options"
string(13) "input_headers"
string(14) "output_headers"
