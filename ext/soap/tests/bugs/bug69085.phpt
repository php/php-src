--TEST--
Bug #69085 (SoapClient's __call() type confusion through unserialize())
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php

#[AllowDynamicProperties]
class MySoapClient extends SoapClient {
    public function __doRequest($request, $location, $action, $version, $one_way = 0): string {
        echo $request, "\n";
        return '';
    }
}
$dummy = unserialize('O:12:"MySoapClient":5:{s:3:"uri";s:1:"a";s:8:"location";s:22:"http://localhost/a.xml";s:17:"__default_headers";i:1337;s:15:"__last_response";s:1:"a";s:5:"trace";s:1:"x";}');
$dummy->whatever();
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Cannot assign int to property SoapClient::$__default_headers of type ?array in %s:%d
Stack trace:
#0 %s(%d): unserialize('O:12:"MySoapCli...')
#1 {main}
  thrown in %s on line %d
