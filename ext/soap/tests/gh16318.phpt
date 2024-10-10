--TEST--
GH-16318 (Recursive array segfaults soap encoding)
--EXTENSIONS--
soap
--FILE--
<?php

// SOAP-ENC array
$tmp =& $test1;
$test1[] = $tmp;

// map array
$test2 = [];
$test2["a"] = "a";
$test2[] =& $test2;

class TestSoapClient extends SoapClient {
    public function __doRequest(string $request, string $location, string $action, int $version, bool $oneWay = false): ?string
    {
        die($request);
    }
}
$client = new TestSoapClient(NULL,array("location"=>"test://","uri"=>"http://soapinterop.org/","trace"=>1,"exceptions"=>0));

foreach ([$test1, $test2] as $test) {
    try {
        $client->__soapCall("echoStructArray", array($test), array("soapaction"=>"http://soapinterop.org/","uri"=>"http://soapinterop.org/"));
    } catch (ValueError $e) {
        echo $e->getMessage(), "\n";
    }
}

?>
--EXPECT--
Recursive array cannot be encoded
Recursive array cannot be encoded
