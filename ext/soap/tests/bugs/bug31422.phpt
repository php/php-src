--TEST--
Bug #31422 (No Error-Logging on SoapServer-Side)
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip not valid for windows');
}
require_once('skipif.inc');
?>
--INI--
log_errors=1
error_log=
--FILE--
<?php
function Add($x,$y) {
	fopen();
	user_error("Hello", E_USER_ERROR);
  return $x+$y;
}

$server = new SoapServer(null,array('uri'=>"http://testuri.org"));
$server->addfunction("Add");

$HTTP_RAW_POST_DATA = <<<EOF
<?xml version="1.0" encoding="ISO-8859-1"?>
<SOAP-ENV:Envelope
  SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"
  xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
  xmlns:xsd="http://www.w3.org/2001/XMLSchema"
  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
  xmlns:si="http://soapinterop.org/xsd">
  <SOAP-ENV:Body>
    <ns1:Add xmlns:ns1="http://testuri.org">
      <x xsi:type="xsd:int">22</x>
      <y xsi:type="xsd:int">33</y>
    </ns1:Add>
  </SOAP-ENV:Body>
</SOAP-ENV:Envelope>
EOF;

$server->handle($HTTP_RAW_POST_DATA);
echo "ok\n";
?>
--EXPECTF--
PHP Warning:  fopen() expects at least 2 parameters, 0 given in %sbug31422.php on line %d
PHP Fatal error:  Hello in %sbug31422.php on line %d
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Server</faultcode><faultstring>Hello</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>
