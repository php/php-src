--TEST--
Bug #77088 (Segfault when using SoapClient with null options)
--EXTENSIONS--
soap
--FILE--
<?php

try
{
    $options = NULL;
    $sClient = new SoapClient("test.wsdl", $options);
}
catch(TypeError $e)
{
    var_dump($e);
}

?>
--EXPECTF--
object(TypeError)#%d (%d) {
  ["message":protected]=>
  string(%d) "SoapClient::__construct(): Argument #2 ($options) must be of type array, null given"
  ["string":"Error":private]=>
  string(0) ""
  ["code":protected]=>
  int(0)
  ["file":protected]=>
  string(%d) "%sbug77088.php"
  ["line":protected]=>
  int(6)
  ["trace":"Error":private]=>
  array(1) {
    [0]=>
    array(6) {
      ["file"]=>
      string(%d) "%sbug77088.php"
      ["line"]=>
      int(6)
      ["function"]=>
      string(11) "__construct"
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
  ["previous":"Error":private]=>
  NULL
}
