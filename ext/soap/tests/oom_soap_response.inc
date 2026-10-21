<?php

/* Builds a compact SOAP response for OOM tests */
function oom_soap_response(int $dimensions, int $items): string
{
    $arrayType = 'xsd:string[' . implode(',', array_fill(0, $dimensions, 1)) . ']';
    $items = str_repeat('<i/>', $items);

    return <<<XML
        <?xml version="1.0"?>
        <SOAP-ENV:Envelope
            xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
            xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/"
            xmlns:xsd="http://www.w3.org/2001/XMLSchema"
            xmlns:ns1="http://test-uri/">
          <SOAP-ENV:Body>
            <ns1:fResponse><p SOAP-ENC:arrayType="$arrayType">$items</p></ns1:fResponse>
          </SOAP-ENV:Body>
        </SOAP-ENV:Envelope>
        XML;
}

/* A SoapClient whose transport is replaced by a canned response, so that the
 * reproducer exercises only the response decoder. */
class OomSoapClient extends SoapClient
{
    public string $response = '';

    public function __doRequest($request, $location, $action, $version, $oneWay = false, $uriParserClass = null): ?string
    {
        return $this->response;
    }
}
