--TEST--
GH-22895 (Heap use-after-free while encoding a Traversable with an illegal key)
--CREDITS--
Amorsec
--EXTENSIONS--
soap
--FILE--
<?php
class LocalSoapClient extends SoapClient
{
    public function __doRequest(
        $request,
        $location,
        $action,
        $version,
        $one_way = false
    ): ?string {
        return '';
    }
}

class ArrayKeyIterator implements Iterator
{
    private int $i = 0;

    public function current(): mixed
    {
        return new stdClass();
    }

    public function key(): mixed
    {
        return ['illegal', 'key'];
    }

    public function next(): void
    {
        $this->i++;
    }

    public function rewind(): void
    {
        $this->i = 0;
    }

    public function valid(): bool
    {
        return $this->i < 2;
    }
}

$client = new LocalSoapClient(null, [
    'location' => 'http://127.0.0.1/',
    'uri' => 'urn:audit',
    'trace' => 1,
]);

$multiple = new MultipleIterator();
$multiple->attachIterator(new ArrayIterator([0]));

foreach ([$multiple, new ArrayKeyIterator()] as $iterator) {
    try {
        $client->__soapCall('audit', [new SoapVar($iterator, SOAP_ENC_ARRAY)]);
    } catch (TypeError $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}

/* A key the encoder can use must still be serialized, without leaking. */
$client->__soapCall('audit', [new SoapVar(new ArrayIterator(['a' => 1]), SOAP_ENC_ARRAY)]);
echo $client->__getLastRequest();
?>
--EXPECT--
TypeError: Cannot access offset of type array on array
TypeError: Cannot access offset of type array on array
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="urn:audit" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:audit><param0 SOAP-ENC:arrayType="xsd:int[1]" xsi:type="SOAP-ENC:Array"><item xsi:type="xsd:int">1</item></param0></ns1:audit></SOAP-ENV:Body></SOAP-ENV:Envelope>
