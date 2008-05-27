--TEST--
Bug #29236 (memory error when wsdl-cache is enabled)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
soap.wsdl_cache_enabled=1
--FILE--
<?php
$client = new SoapClient(dirname(__FILE__)."/bug29236.wsdl");
var_dump($client->__getFunctions()); 
?>
--EXPECT--
array(4) {
  [0]=>
  unicode(59) "StartSessionResponse StartSession(StartSession $parameters)"
  [1]=>
  unicode(62) "VerifySessionResponse VerifySession(VerifySession $parameters)"
  [2]=>
  unicode(41) "LogoutResponse Logout(Logout $parameters)"
  [3]=>
  unicode(62) "GetSystemInfoResponse GetSystemInfo(GetSystemInfo $parameters)"
}
