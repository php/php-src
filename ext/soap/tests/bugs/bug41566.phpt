--TEST--
Bug #41566 (SOAP Server not properly generating href attributes)
--EXTENSIONS--
soap
--FILE--
<?php
function test() {
  $aUser = new User();
  $aUser->sName = 'newUser';

  $aUsers = Array();
  $aUsers[] = $aUser;
  $aUsers[] = $aUser;
  $aUsers[] = $aUser;
  $aUsers[] = $aUser;
  return $aUsers;
}

/* Simple User definition */
Class User {
  /** @var string */
  public $sName;
}

$server = new soapserver(null,array('uri'=>"http://testuri.org", 'soap_version'=>SOAP_1_2));
$server->addfunction("test");

$HTTP_RAW_POST_DATA = <<<EOF
<?xml version="1.0" encoding="ISO-8859-1"?>
<SOAP-ENV:Envelope
  SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"
  xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
  xmlns:xsd="http://www.w3.org/2001/XMLSchema"
  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
  xmlns:si="http://soapinterop.org/xsd">
  <SOAP-ENV:Body>
    <ns1:test xmlns:ns1="http://testuri.org" />
  </SOAP-ENV:Body>
</SOAP-ENV:Envelope>
EOF;
ob_start();
$server->handle($HTTP_RAW_POST_DATA);
echo "ok\n";

$HTTP_RAW_POST_DATA = <<<EOF
<?xml version="1.0" encoding="UTF-8"?>
<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope"
  xmlns:ns1="http://testuri.org"
  xmlns:xsd="http://www.w3.org/2001/XMLSchema"
  xmlns:enc="http://www.w3.org/2003/05/soap-encoding">
  <env:Body>
    <ns1:test env:encodingStyle="http://www.w3.org/2003/05/soap-encoding"/>
  </env:Body>
</env:Envelope>
EOF;
$server->handle($HTTP_RAW_POST_DATA);
echo "ok\n";
ob_flush();
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://testuri.org" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:testResponse><return SOAP-ENC:arrayType="SOAP-ENC:Struct[4]" xsi:type="SOAP-ENC:Array"><item xsi:type="SOAP-ENC:Struct" id="ref1"><sName xsi:type="xsd:string">newUser</sName></item><item href="#ref1"/><item href="#ref1"/><item href="#ref1"/></return></ns1:testResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
<?xml version="1.0" encoding="UTF-8"?>
<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope" xmlns:ns1="http://testuri.org" xmlns:enc="http://www.w3.org/2003/05/soap-encoding" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"><env:Body xmlns:rpc="http://www.w3.org/2003/05/soap-rpc"><ns1:testResponse env:encodingStyle="http://www.w3.org/2003/05/soap-encoding"><rpc:result>return</rpc:result><return enc:itemType="enc:Struct" enc:arraySize="4" xsi:type="enc:Array"><item xsi:type="enc:Struct" enc:id="ref1"><sName xsi:type="xsd:string">newUser</sName></item><item enc:ref="#ref1"/><item enc:ref="#ref1"/><item enc:ref="#ref1"/></return></ns1:testResponse></env:Body></env:Envelope>
ok
