--TEST--
Bug #65018 (SoapHeader problems with SoapServer)
--EXTENSIONS--
soap
--FILE--
<?php
    class Tool{
        public function TOKEN($id){
            return new SoapHeader('namespace1', 'TOKEN', $id, true);
        }
        public function Method(){}
    }

    $input = $input =
        '<?xml version="1.0"?>'.PHP_EOL.
        '<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="namespace1"'.
        ' xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"'.
        ' xmlns:xsd="http://www.w3.org/2001/XMLSchema">'.
        '<SOAP-ENV:Header><ns1:TOKEN soapenv:mustUnderstand="1">abc</ns1:TOKEN></SOAP-ENV:Header>'.
        '<SOAP-ENV:Body><ns1:Method /></SOAP-ENV:Body></SOAP-ENV:Envelope>';

    $soap = new SoapServer(null, array('uri' => '127.0.0.1'));
    $soap->setClass('Tool');
    $soap->handle($input);
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="namespace1" xmlns:ns2="127.0.0.1" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Header><ns1:TOKEN SOAP-ENV:mustUnderstand="1">abc</ns1:TOKEN></SOAP-ENV:Header><SOAP-ENV:Body><ns2:MethodResponse><return xsi:nil="true"/></ns2:MethodResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
