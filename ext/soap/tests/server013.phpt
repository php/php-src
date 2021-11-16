--TEST--
SOAP Server 13: array handling
--EXTENSIONS--
soap
--FILE--
<?php
function Sum($a) {
  $sum = 0;
  if (is_array($a)) {
    foreach($a as $val) {
      $sum += $val;
    }
  }
  return $sum;
}

$server = new soapserver(null,array('uri'=>"http://testuri.org"));
$server->addfunction("Sum");

$HTTP_RAW_POST_DATA = <<<EOF
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope
  xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
  SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"
  xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/"
  xmlns:xsd="http://www.w3.org/2001/XMLSchema"
  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
  <SOAP-ENV:Body xmlns:ns1="http://linuxsrv.home/~dmitry/soap/">
    <ns1:sum>
      <param0 SOAP-ENC:arrayType="xsd:int[2]" xsi:type="SOAP-ENC:Array">
        <val xsi:type="xsd:int">3</val>
        <val xsi:type="xsd:int">5</val>
      </param0>
    </ns1:sum>
  </SOAP-ENV:Body>
</SOAP-ENV:Envelope>
EOF;
$server->handle($HTTP_RAW_POST_DATA);
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://testuri.org" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:sumResponse><return xsi:type="xsd:int">8</return></ns1:sumResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
