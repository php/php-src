--TEST--
Bug #30045 (Cannot pass big integers (> 2147483647) in SOAP requests)
--EXTENSIONS--
soap
simplexml
--INI--
soap.wsdl_cache_enabled=1
--FILE--
<?php

function foo($type, $num) {
  return new SoapVar($num, $type);
}

class LocalSoapClient extends SoapClient {
  private $server;

  function __construct($wsdl, $options) {
    parent::__construct($wsdl, $options);
    $this->server = new SoapServer($wsdl, $options);
    $this->server->addFunction('foo');
  }

  function __doRequest($request, $location, $action, $version, $one_way = 0): ?string {
    $xml = simplexml_load_string($request);
    echo $xml->children("http://schemas.xmlsoap.org/soap/envelope/")->Body->children("http://test-uri")->children()->param1->asXML(),"\n";
    unset($xml);

    ob_start();
    $this->server->handle($request);
    $response = ob_get_contents();
    ob_end_clean();

    return $response;
  }

}

$soap = new LocalSoapClient(NULL, array("uri"=>"http://test-uri", "location"=>"test://"));

function test($type, $num) {
  global $soap;
  try {
      printf("  %0.0f\n    ", $num);
      $ret = $soap->foo($type, new SoapVar($num, $type));
      printf("    %0.0f\n", $ret);
    } catch (SoapFault $ex) {
      var_dump($ex);
    }
}
/*
echo "byte\n";
//test(XSD_BYTE, -129);
test(XSD_BYTE, -128);
test(XSD_BYTE,  127);
//test(XSD_BYTE,  128);

echo "\nshort\n";
//test(XSD_SHORT, -32769);
test(XSD_SHORT, -32768);
test(XSD_SHORT,  32767);
//test(XSD_SHORT,  32768);

echo "\nint\n";
//test(XSD_INT, -2147483649);
test(XSD_INT, -2147483648);
test(XSD_INT,  2147483647);
//test(XSD_INT,  2147483648);

echo "\nlong\n";
//test(XSD_LONG, -9223372036854775809);
test(XSD_LONG, -9223372036854775808);
test(XSD_LONG,  9223372036854775807);
//test(XSD_LONG,  9223372036854775808);

echo "\nunsignedByte\n";
//test(XSD_UNSIGNEDBYTE, -1);
test(XSD_UNSIGNEDBYTE,  0);
test(XSD_UNSIGNEDBYTE,  255);
//test(XSD_UNSIGNEDBYTE,  256);

echo "\nunsignedShort\n";
//test(XSD_UNSIGNEDSHORT, -1);
test(XSD_UNSIGNEDSHORT,  0);
test(XSD_UNSIGNEDSHORT,  65535);
//test(XSD_UNSIGNEDSHORT,  65536);

echo "\nunsignedInt\n";
//test(XSD_UNSIGNEDINT, -1);
test(XSD_UNSIGNEDINT,  0);
test(XSD_UNSIGNEDINT,  4294967295);
//test(XSD_UNSIGNEDINT,  4294967296);

echo "\nunsignedLong\n";
//test(XSD_UNSIGNEDLONG, -1);
test(XSD_UNSIGNEDLONG,  0);
test(XSD_UNSIGNEDLONG,  18446744073709551615);
//test(XSD_UNSIGNEDLONG,  18446744073709551616);

echo "\nnegativeInteger\n";
test(XSD_NEGATIVEINTEGER, -18446744073709551616);
test(XSD_NEGATIVEINTEGER, -1);
//test(XSD_NEGATIVEINTEGER,  0);

echo "\nnonPositiveInteger\n";
test(XSD_NONPOSITIVEINTEGER, -18446744073709551616);
test(XSD_NONPOSITIVEINTEGER,  0);
//test(XSD_NONPOSITIVEINTEGER,  1);

echo "\nnonNegativeInteger\n";
//test(XSD_NONNEGATIVEINTEGER, -1);
test(XSD_NONNEGATIVEINTEGER,  0);
test(XSD_NONNEGATIVEINTEGER,  18446744073709551616);

echo "\nPositiveInteger\n";
//test(XSD_POSITIVEINTEGER,  0);
test(XSD_POSITIVEINTEGER,  1);
test(XSD_POSITIVEINTEGER,  18446744073709551616);

echo "\ninteger\n";
test(XSD_INTEGER, -18446744073709551616);
test(XSD_INTEGER,  18446744073709551616);
*/
echo "long\n";
test(XSD_LONG, 2147483647);
test(XSD_LONG, 2147483648);
test(XSD_LONG,  4294967296);
test(XSD_LONG,  8589934592);
test(XSD_LONG, 17179869184);

echo "\nunsignedLong\n";
test(XSD_UNSIGNEDLONG,  2147483647);
test(XSD_UNSIGNEDLONG,  2147483648);
test(XSD_UNSIGNEDLONG,  4294967296);
test(XSD_UNSIGNEDLONG,  8589934592);
test(XSD_UNSIGNEDLONG, 17179869184);

?>
--EXPECT--
long
  2147483647
    <param1 xsi:type="xsd:long">2147483647</param1>
    2147483647
  2147483648
    <param1 xsi:type="xsd:long">2147483648</param1>
    2147483648
  4294967296
    <param1 xsi:type="xsd:long">4294967296</param1>
    4294967296
  8589934592
    <param1 xsi:type="xsd:long">8589934592</param1>
    8589934592
  17179869184
    <param1 xsi:type="xsd:long">17179869184</param1>
    17179869184

unsignedLong
  2147483647
    <param1 xsi:type="xsd:unsignedLong">2147483647</param1>
    2147483647
  2147483648
    <param1 xsi:type="xsd:unsignedLong">2147483648</param1>
    2147483648
  4294967296
    <param1 xsi:type="xsd:unsignedLong">4294967296</param1>
    4294967296
  8589934592
    <param1 xsi:type="xsd:unsignedLong">8589934592</param1>
    8589934592
  17179869184
    <param1 xsi:type="xsd:unsignedLong">17179869184</param1>
    17179869184
