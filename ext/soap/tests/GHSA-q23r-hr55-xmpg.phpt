--TEST--
GHSA-q23r-hr55-xmpg: Out-of-bounds read on empty SOAP-ENC:arraySize
--EXTENSIONS--
soap
--FILE--
<?php

function echo_back($data) { $GLOBALS['decoded'][] = $data; return null; }

function request(string $arraySize, string $itemType = ''): string {
    $itemTypeAttribute = $itemType === '' ? '' : "SOAP-ENC:itemType=\"{$itemType}\"";

    return <<<XML
        <?xml version="1.0"?>
        <SOAP-ENV:Envelope
            xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
            xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:xsd="http://www.w3.org/2001/XMLSchema">

            <SOAP-ENV:Body SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
                <echo_back>
                    <data xsi:type="SOAP-ENC:Array" {$itemTypeAttribute} SOAP-ENC:arraySize="{$arraySize}">
                        <item xsi:type="xsd:string">A</item>
                        <item xsi:type="xsd:string">B</item>
                        <item xsi:type="xsd:string">C</item>
                    </data>
                </echo_back>
            </SOAP-ENV:Body>
        </SOAP-ENV:Envelope>
        XML;
}

function decode(SoapServer $server, string $request): string {
    ob_start();
    $server->handle($request);
    $response = ob_get_clean();

    if (str_contains($response, '<SOAP-ENV:Fault>')) {
        return 'fault: ' . strip_tags($response);
    }

    return json_encode(array_pop($GLOBALS['decoded']));
}

$server = new SoapServer(null, ['uri' => 'http://test']);
$server->addFunction('echo_back');

$GLOBALS['decoded'] = [];

foreach (['', ' ', 'abc', '*', '3', '2 2'] as $arraySize) {
    echo var_export($arraySize, true), ' => ', decode($server, request($arraySize)), "\n";
}

echo "itemType => ", decode($server, request('', 'xsd:string')), "\n";

?>
--EXPECT--
'' => ["A","B","C"]
' ' => ["A","B","C"]
'abc' => ["A","B","C"]
'*' => ["A","B","C"]
'3' => ["A","B","C"]
'2 2' => [["A","B"],["C"]]
itemType => ["A","B","C"]
