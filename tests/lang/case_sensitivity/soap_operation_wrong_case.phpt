--TEST--
SoapClient WSDL operation names are matched case-sensitively
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
class LocalClient extends SoapClient {
    public function __doRequest(string $request, string $location, string $action, int $version, bool $oneWay = false, ?string $uriParserClass = null): ?string {
        return '<?xml version="1.0" encoding="UTF-8"?>'
            . '<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"'
            . ' xmlns:ns1="http://soapinterop.org/" xmlns:xsd="http://www.w3.org/2001/XMLSchema"'
            . ' xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"><SOAP-ENV:Body>'
            . '<ns1:echoStringResponse><return xsi:type="xsd:string">ok</return></ns1:echoStringResponse>'
            . '</SOAP-ENV:Body></SOAP-ENV:Envelope>';
    }
}

$client = new LocalClient(__DIR__ . '/../../../ext/soap/tests/interop/Round2/Base/round2_base.wsdl', ['exceptions' => true]);

// Exact case works.
var_dump($client->echoString('Hello'));

// Wrong case is not a WSDL operation.
try {
    $client->echostring('Hello');
} catch (SoapFault $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
string(2) "ok"
Function ("echostring") is not a valid method for this service
