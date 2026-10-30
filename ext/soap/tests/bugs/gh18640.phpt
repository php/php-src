--TEST---
GH-18640 (heap-use-after-free ext/soap/php_encoding.c:299:32 in soap_check_zval_ref)
--EXTENSIONS--
soap
--CREDITS--
YuanchengJiang
--FILE--
<?php
$wsdl = __DIR__."/bug35142.wsdl";

class TestSoapClient extends SoapClient {
    function __doRequest($request, $location, $action, $version, $one_way = 0): ?string {
        var_dump($request);
        return '';
    }
}

$soapClient = new TestSoapClient($wsdl, ['trace' => 1, 'classmap' => ['logOnEvent' => 'LogOnEvent', 'events' => 'IVREvents']]);
$timestamp = new LogOnEvent(); // Bogus!
$logOffEvents[] = new LogOffEvent($timestamp);
$logOffEvents[] = new LogOffEvent($timestamp);
$ivrEvents = new IVREvents($logOffEvents);
$result = $soapClient->PostEvents($ivrEvents);

class LogOffEvent {
    function __construct(public $timestamp) {
        $this->timestamp = $timestamp;
    }
}

class LogOnEvent {
}

class IVREvents {
    function __construct(public $logOffEvent) {
    }
}
?>
--EXPECT--
string(359) "<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://testurl/Events" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ns2="http://testurl/Message"><SOAP-ENV:Body><ns2:ivrEvents><ns2:logOffEvent/><ns2:logOffEvent/></ns2:ivrEvents></SOAP-ENV:Body></SOAP-ENV:Envelope>
"
