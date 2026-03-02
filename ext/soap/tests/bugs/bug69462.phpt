--TEST--
Bug #69462 (__soapCall with a SoapVar that has a namespace but no name crashes)
--EXTENSIONS--
soap
--FILE--
<?php
$namespace = "http://example.com/ns";

$client = new SoapClient(null, [ 'exceptions' => 1, 'location' => "", 'uri' => $namespace ]);

$soapvar = new SoapVar(
    array(
        new SoapVar('value', XSD_STRING, null, null, null, $namespace)
    ),
    SOAP_ENC_OBJECT, null, null, 'name', $namespace
);

try {
    $client->__soapCall('method', array($soapvar));
}
catch (Exception $e) {
    /* ignore any errors, we're testing for segmentation fault anyway */
    echo "good";
}
?>
--EXPECT--
good
