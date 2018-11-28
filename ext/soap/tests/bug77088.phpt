--TEST--
Bug #77088 (Segfault when using SoapClient with null options)
--SKIPIF--
<?php
	require_once('skipif.inc');
?>
--FILE--
<?php

try
{
    $options = NULL;
    $sClient = new SoapClient("test.wsdl", $options);
} 
catch(SoapFault $e)
{
    var_dump($e);
}

?>
--EXPECTF--
Warning: SoapClient::SoapClient() expects parameter 2 to be array, null given in %sbug77088.php on line %d
object(SoapFault)#%d (%d) {
  ["message":protected]=>
  string(44) "SoapClient::SoapClient(): Invalid parameters"
  ["string":"Exception":private]=>
  string(0) ""
  ["code":protected]=>
  int(0)
  ["file":protected]=>
  string(%d) "%sbug77088.php"
  ["line":protected]=>
  int(6)
  ["trace":"Exception":private]=>
  array(1) {
    [0]=>
    array(6) {
      ["file"]=>
      string(%d) "%sbug77088.php"
      ["line"]=>
      int(6)
      ["function"]=>
      string(10) "SoapClient"
      ["class"]=>
      string(10) "SoapClient"
      ["type"]=>
      string(2) "->"
      ["args"]=>
      array(2) {
        [0]=>
        string(9) "test.wsdl"
        [1]=>
        NULL
      }
    }
  }
  ["previous":"Exception":private]=>
  NULL
  ["faultstring"]=>
  string(44) "SoapClient::SoapClient(): Invalid parameters"
  ["faultcode"]=>
  string(6) "Client"
  ["faultcodens"]=>
  string(41) "http://schemas.xmlsoap.org/soap/envelope/"
}
