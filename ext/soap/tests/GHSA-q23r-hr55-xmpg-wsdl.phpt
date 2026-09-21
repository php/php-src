--TEST--
GHSA-q23r-hr55-xmpg: Unbounded array walk on empty wsdl:arraySize
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php

const WSDL = __DIR__ . '/GHSA-q23r-hr55-xmpg-wsdl.wsdl';

function itemType() { return ['A', 'B', 'C']; }
function sizeOnly() { return ['A', 'B', 'C']; }

function request(string $operation): string {
    return <<<XML
        <?xml version="1.0"?>
        <env:Envelope
            xmlns:env="http://www.w3.org/2003/05/soap-envelope"
            xmlns:m="urn:arraysize">

            <env:Body>
                <m:{$operation}/>
            </env:Body>
        </env:Envelope>
        XML;
}

class ReplayClient extends SoapClient
{
    public function __construct(public string $arrayType) {
        parent::__construct(WSDL);
    }

    public function __doRequest($request, $location, $action, $version, $oneWay = false, ?string $uriParserClass = null): ?string {
        return <<<XML
            <?xml version="1.0"?>
            <env:Envelope
                xmlns:env="http://www.w3.org/2003/05/soap-envelope"
                xmlns:ns1="urn:arraysize"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xmlns:xsd="http://www.w3.org/2001/XMLSchema">

                <env:Body>
                    <ns1:{$this->arrayType}Response>
                        <result xsi:type="ns1:{$this->arrayType}Array">
                            <item xsi:type="xsd:string">A</item>
                            <item xsi:type="xsd:string">B</item>
                            <item xsi:type="xsd:string">C</item>
                        </result>
                    </ns1:{$this->arrayType}Response>
                </env:Body>
            </env:Envelope>
            XML;
    }
}

$server = new SoapServer(WSDL);
$server->addFunction(['itemType', 'sizeOnly']);

foreach (['itemType', 'sizeOnly'] as $operation) {
    ob_start();
    $server->handle(request($operation));
    $response = ob_get_clean();

    preg_match('/arraySize="[^"]*"/', $response, $matches);
    echo 'encode ', $operation, ' => ', $matches[0] ?? 'none', "\n";
}

foreach (['ItemType', 'SizeOnly'] as $arrayType) {
    $client = new ReplayClient($arrayType);
    echo 'decode ', $arrayType, ' => ', json_encode($client->{lcfirst($arrayType)}()), "\n";
}

?>
--EXPECT--
encode itemType => arraySize="3"
encode sizeOnly => arraySize="3"
decode ItemType => ["A","B","C"]
decode SizeOnly => ["A","B","C"]
