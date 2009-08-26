--TEST--
Bug #47273 (Encoding bug in SoapServer->fault)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
unicode.script_encoding=ISO-8859-1
unicode.output_encoding=ISO-8859-1
--FILE--
<?php
$request1 = <<<EOF
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://127.0.0.1:8080/test/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:test1/></SOAP-ENV:Body></SOAP-ENV:Envelope>
EOF;
$request2 = <<<EOF
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://127.0.0.1:8080/test/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:test2/></SOAP-ENV:Body></SOAP-ENV:Envelope>
EOF;

class SoapFaultTest
{
    public function test1() {
    	//  Test #1
        return 'Test #1 exception with some special chars: Äßö';
    }
    public function test2() {    
        //  Test #2
	//throw new SoapFault('Server', 'Test #2 exception with some special chars: Äßö');
        throw new Exception('Test #2 exception with some special chars: Äßö');
    }
}

$server = new SoapServer(null, array(
'uri' => "http://127.0.0.1:8080/test/",
'encoding' => 'ISO-8859-1'));
$server->setClass('SoapFaultTest');

try {
	$server->handle($request1);
} catch (Exception $e) {
	$server->fault("Sender", $e->getMessage());
}
try {
        $server->handle($request2);
} catch (Exception $e) {
        $server->fault("Sender", $e->getMessage());
}
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://127.0.0.1:8080/test/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:test1Response><return xsi:type="xsd:string">Test #1 exception with some special chars: ÃÃÃ¶</return></ns1:test1Response></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>Sender</faultcode><faultstring>Test #2 exception with some special chars: ÃÃÃ¶</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>

