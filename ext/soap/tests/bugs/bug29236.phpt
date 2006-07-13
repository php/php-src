--TEST--
Bug #29236 (memory error when wsdl-cache is enabled)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$client = new SoapClient(dirname(__FILE__)."/bug29236.wsdl");
var_dump($client->__getFunctions()); 
?>
--EXPECT--
array(4) {
  [0]=>
  string(59) "StartSessionResponse StartSession(StartSession $parameters)"
  [1]=>
  string(62) "VerifySessionResponse VerifySession(VerifySession $parameters)"
  [2]=>
  string(41) "LogoutResponse Logout(Logout $parameters)"
  [3]=>
  string(62) "GetSystemInfoResponse GetSystemInfo(GetSystemInfo $parameters)"
}
--UEXPECT--
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
