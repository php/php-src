<?php
//
// +----------------------------------------------------------------------+
// | PHP Version 4                                                        |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2003 The PHP Group                                |
// +----------------------------------------------------------------------+
// | This source file is subject to version 2.02 of the PHP license,      |
// | that is bundled with this package in the file LICENSE, and is        |
// | available at through the world-wide-web at                           |
// | http://www.php.net/license/2_02.txt.                                 |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Authors: Shane Caraveo <Shane@Caraveo.com>   Port to PEAR and more   |
// +----------------------------------------------------------------------+
//
// $Id$
//

require_once 'SOAP/Server.php';

$server = new SOAP_Server;

require_once 'server_round2_base.php';
require_once 'server_round2_groupB.php';
require_once 'server_round2_groupC.php';

$test = '<?xml version="1.0"?>

<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
 xmlns:xsd="http://www.w3.org/2001/XMLSchema"
 xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
 xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/"
 xmlns:si="http://soapinterop.org/xsd"
 xmlns:ns6="http://soapinterop.org/echoheader/"
  SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
<SOAP-ENV:Header>

<ns6:echoMeStringRequest xsi:type="xsd:string" SOAP-ENV:actor="http://schemas.xmlsoap.org/soap/actor/next" SOAP-ENV:mustUnderstand="0">hello world</ns6:echoMeStringRequest>
</SOAP-ENV:Header>
<SOAP-ENV:Body>

<echoVoid></echoVoid>
</SOAP-ENV:Body>
</SOAP-ENV:Envelope>';

$test = '<?xml version="1.0"?>

<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
 xmlns:xsd="http://www.w3.org/2001/XMLSchema"
 xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
 xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/"
 xmlns:si="http://soapinterop.org/xsd"
 xmlns:ns6="http://soapinterop.org/echoheader/"
  SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
<SOAP-ENV:Header>

<ns6:echoMeStructRequest xsi:type="si:SOAPStruct"
 SOAP-ENV:actor="http://schemas.xmlsoap.org/soap/actor/next"
 SOAP-ENV:mustUnderstand="1">
<varString xsi:type="xsd:string">arg</varString>

<varInt xsi:type="xsd:int">34</varInt>

<varFloat xsi:type="xsd:float">325.325</varFloat>
</ns6:echoMeStructRequest>
</SOAP-ENV:Header>
<SOAP-ENV:Body>

<echoVoid></echoVoid>
</SOAP-ENV:Body>
</SOAP-ENV:Envelope>
';

$test = '<?xml version="1.0"?>

<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
 xmlns:xsd="http://www.w3.org/2001/XMLSchema"
 xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
 xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/"
 xmlns:si="http://soapinterop.org/xsd"
 xmlns:ns6="http://soapinterop.org/echoheader/"
 xmlns:ns7="http://soapinterop.org/"
  SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
<SOAP-ENV:Body>

<ns7:echoString>
<inputString xsi:type="xsd:string"></inputString>
</ns7:echoString>
</SOAP-ENV:Body>
</SOAP-ENV:Envelope>
';
$test = '<?xml version="1.0" encoding="US-ASCII"?>

<SOAP-ENV:Envelope  xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
  xmlns:xsd="http://www.w3.org/2001/XMLSchema"
  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
  xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/"
  xmlns:si="http://soapinterop.org/xsd"
  xmlns:ns6="http://soapinterop.org/"
  SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
<SOAP-ENV:Body>
<ns6:echoVoid/>
</SOAP-ENV:Body>
</SOAP-ENV:Envelope>';

$test = '<?xml version="1.0" encoding="US-ASCII"?>

<SOAP-ENV:Envelope  xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
  xmlns:xsd="http://www.w3.org/2001/XMLSchema"
  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
  xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/"
  xmlns:si="http://soapinterop.org/xsd"
  xmlns:ns6="http://soapinterop.org/"
  SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
<SOAP-ENV:Body>
<ns6:echoIntegerArray><inputIntegerArray xsi:type="SOAP-ENC:Array" SOAP-ENC:arrayType="xsd:int[3]" SOAP-ENC:offset="[0]"><item xsi:type="xsd:int">1</item>
<item xsi:type="xsd:int">234324324</item>
<item xsi:type="xsd:int">2</item>
</inputIntegerArray>
</ns6:echoIntegerArray>
</SOAP-ENV:Body>
</SOAP-ENV:Envelope>';

#$test = "<S:Envelope
#S:encodingStyle='http://schemas.xmlsoap.org/soap/encoding/'
#xmlns:Enc='http://schemas.xmlsoap.org/soap/encoding/'
#xmlns:S='http://schemas.xmlsoap.org/soap/envelope/'
#xmlns:a='http://soapinterop.org/'
#xmlns:b='http://soapinterop.org/xsd'
#xmlns:XS='http://www.w3.org/2001/XMLSchema'
#xmlns:XI='http://www.w3.org/2001/XMLSchema-instance'>
#<S:Body>
#<b:SOAPStruct Enc:root='0' id='21b56c4' XI:type='b:SOAPStruct'>
#<varInt XI:type='XS:int'>1</varInt>
#<varFloat XI:type='XS:float'>2</varFloat>
#<varString XI:type='XS:string'>wilma</varString>
#</b:SOAPStruct>
#<a:echoStructArray>
#<inputStructArray XI:type='Enc:Array' Enc:arrayType='XS:anyType[3]'>
#<fred href='#21b56c4'/>
#<i href='#21b56c4'/>
#<i href='#21b56c4'/>
#</inputStructArray>
#</a:echoStructArray>
#</S:Body></S:Envelope>";

#$test = "<S:Envelope S:encodingStyle='http://schemas.xmlsoap.org/soap/encoding/' xmlns:Enc='http://schemas.xmlsoap.org/soap/encoding/' xmlns:S='http://schemas.xmlsoap.org/soap/envelope/' xmlns:a='http://soapinterop.org/' xmlns:b='http://soapinterop.org/xsd' xmlns:XS='http://www.w3.org/2001/XMLSchema' xmlns:XI='http://www.w3.org/2001/XMLSchema-instance'> <S:Body><a:echoStructArray><inputStructArray XI:type='Enc:Array' Enc:arrayType='b:SOAPStruct[2]'><inputStruct href='#213e654'/> <inputStruct href='#21b8c4c'/> </inputStructArray> </a:echoStructArray> <b:SOAPStruct Enc:root='0' id='21b8c4c' XI:type='b:SOAPStruct'><varInt XI:type='XS:int'>-1</varInt> <varFloat XI:type='XS:float'>-1</varFloat> <varString XI:type='XS:string'>lean on into the groove y'all</varString> </b:SOAPStruct> <b:SOAPStruct Enc:root='0' id='213e654' XI:type='b:SOAPStruct'><varInt XI:type='XS:int'>1073741824</varInt> <varFloat XI:type='XS:float'>-42.24</varFloat> <varString XI:type='XS:string'>pocketSOAP rocks!&lt;g&gt;</varString> </b:SOAPStruct> </S:Body></S:Envelope>";

#$test = "<S:Envelope S:encodingStyle='http://schemas.xmlsoap.org/soap/encoding/' xmlns:S='http://schemas.xmlsoap.org/soap/envelope/' xmlns:b='http://soapinterop.org/' xmlns:a='http://soapinterop.org/headers/' xmlns:XS='http://www.w3.org/2001/XMLSchema' xmlns:XI='http://www.w3.org/2001/XMLSchema-instance'> <S:Header> <a:Transaction S:mustUnderstand='1' XI:type='XS:short'>5</a:Transaction> </S:Header> <S:Body><b:echoString><inputString XI:type='XS:string'>Opps, should never see me</inputString> </b:echoString> </S:Body></S:Envelope>";
#$test = "<S:Envelope S:encodingStyle='http://schemas.xmlsoap.org/soap/encoding/' xmlns:S='http://schemas.xmlsoap.org/soap/envelope/' xmlns:b='http://soapinterop.org/' xmlns:a='http://soapinterop.org/headers/' xmlns:XS='http://www.w3.org/2001/XMLSchema' xmlns:XI='http://www.w3.org/2001/XMLSchema-instance'> <S:Header> <a:Transaction XI:type='XS:short'>5</a:Transaction> </S:Header> <S:Body><b:echoString><inputString XI:type='XS:string'>Opps, should never see me</inputString> </b:echoString> </S:Body></S:Envelope>";
#$test = "<S:Envelope S:encodingStyle='http://schemas.xmlsoap.org/soap/encoding/' xmlns:Enc='http://schemas.xmlsoap.org/soap/encoding/' xmlns:S='http://schemas.xmlsoap.org/soap/envelope/' xmlns:a='http://soapinterop.org/' xmlns:b='http://soapinterop.org/xsd' xmlns:XS='http://www.w3.org/2001/XMLSchema' xmlns:XI='http://www.w3.org/2001/XMLSchema-instance'> <S:Body><a:echoStructAsSimpleTypes><inputStruct href='#213e59c'/> </a:echoStructAsSimpleTypes> <b:SOAPStruct Enc:root='0' id='213e59c' XI:type='b:SOAPStruct'><varInt XI:type='XS:int'>42</varInt> <varString XI:type='XS:string'>Orbital</varString> <varFloat XI:type='XS:float'>-42.42</varFloat> </b:SOAPStruct> </S:Body></S:Envelope>";

// white mesa failures
/*$test = '<?xml version="1.0" encoding="UTF-8"?><SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"><SOAP-ENV:Header><h:echoMeStringRequest SOAP-ENV:actor="http://schemas.xmlsoap.org/soap/actor/next" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:mustUnderstand="1" xmlns:h="http://unknown.org/">White Mesa Test Header String.</h:echoMeStringRequest></SOAP-ENV:Header><SOAP-ENV:Body><m:echoVoid SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:m="http://soapinterop.org/" /></SOAP-ENV:Body></SOAP-ENV:Envelope>';
*/
/*
$test = '<?xml version="1.0" encoding="UTF-8"?><SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"><SOAP-ENV:Header><h:echoMeStringRequest SOAP-ENV:actor="http://schemas.xmlsoap.org/soap/actor/next" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:mustUnderstand="1" xmlns:h="http://unknown.org/">White Mesa Test Header String.</h:echoMeStringRequest></SOAP-ENV:Header><SOAP-ENV:Body><m:echoVoid SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:m="http://soapinterop.org/" /></SOAP-ENV:Body></SOAP-ENV:Envelope>';
*/
//$test = "<S:Envelope S:encodingStyle='http://schemas.xmlsoap.org/soap/encoding/' xmlns:S='http://schemas.xmlsoap.org/soap/envelope/' xmlns:a='http://soapinterop.org/' xmlns:XI='http://www.w3.org/2001/XMLSchema-instance'> <S:Body><a:echoIntegerArray><inputIntegerArray XI:nil='true'></inputIntegerArray> </a:echoIntegerArray> </S:Body></S:Envelope>";

$test = '<?xml version="1.0" encoding="UTF-8"?>

<SOAP-ENV:Envelope  xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
  xmlns:xsd="http://www.w3.org/2001/XMLSchema"
  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
  xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/"
  xmlns:ns4="http://soapinterop.org/"
  SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
<SOAP-ENV:Body>

<ns4:echo2DStringArray>
<input2DStringArray xsi:type="SOAP-ENC:Array" SOAP-ENC:arrayType="xsd:string[2,3]">
<item xsi:type="xsd:string">row0col0</item>
<item xsi:type="xsd:string">row0col1</item>
<item xsi:type="xsd:string">row0col2</item>
<item xsi:type="xsd:string">row1col0</item>
<item xsi:type="xsd:string">row1col1</item>
<item xsi:type="xsd:string">row1col2</item></input2DStringArray></ns4:echo2DStringArray>
</SOAP-ENV:Body>
</SOAP-ENV:Envelope>';

$_SERVER['REQUEST_METHOD'] = "POST";
$_SERVER['CONTENT_TYPE'] = 'multipart/related; type=text/xml; boundary="=_d624611fe466a88d956a205651c74fdb"';

$test = '--=_d624611fe466a88d956a205651c74fdb
Content-Type: text/xml; charset="UTF-8"
Content-Transfer-Encoding: base64

PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iVVRGLTgiPz4NCg0KPFNPQVAtRU5WOkVudmVs
b3BlICB4bWxuczpTT0FQLUVOVj0iaHR0cDovL3NjaGVtYXMueG1sc29hcC5vcmcvc29hcC9lbnZl
bG9wZS8iDQogIHhtbG5zOnhzZD0iaHR0cDovL3d3dy53My5vcmcvMjAwMS9YTUxTY2hlbWEiDQog
IHhtbG5zOnhzaT0iaHR0cDovL3d3dy53My5vcmcvMjAwMS9YTUxTY2hlbWEtaW5zdGFuY2UiDQog
IHhtbG5zOlNPQVAtRU5DPSJodHRwOi8vc2NoZW1hcy54bWxzb2FwLm9yZy9zb2FwL2VuY29kaW5n
LyINCiAgU09BUC1FTlY6ZW5jb2RpbmdTdHlsZT0iaHR0cDovL3NjaGVtYXMueG1sc29hcC5vcmcv
c29hcC9lbmNvZGluZy8iPg0KPFNPQVAtRU5WOkJvZHk+DQoNCjxlY2hvTWltZUF0dGFjaG1lbnQ+
DQo8dGVzdCBocmVmPSJjaWQ6ZTZiOTg0M2JiYzUxY2JiMDAzOTk0MGVmN2VlNzY2MDMiLz48L2Vj
aG9NaW1lQXR0YWNobWVudD4NCjwvU09BUC1FTlY6Qm9keT4NCjwvU09BUC1FTlY6RW52ZWxvcGU+
DQo=
--=_d624611fe466a88d956a205651c74fdb
Content-Disposition: attachment.php
Content-Type: text/plain
Content-Transfer-Encoding: base64
Content-ID: <e6b9843bbc51cbb0039940ef7ee76603>

PD9waHANCnJlcXVpcmVfb25jZSgiU09BUC9DbGllbnQucGhwIik7DQpyZXF1aXJlX29uY2UoIlNP
QVAvdGVzdC90ZXN0LnV0aWxpdHkucGhwIik7DQpyZXF1aXJlX29uY2UoIlNPQVAvVmFsdWUucGhw
Iik7DQokc29hcF9iYXNlID0gbmV3IFNPQVBfQmFzZSgpOw0KDQokdiA9ICBuZXcgU09BUF9BdHRh
Y2htZW50KCd0ZXN0JywndGV4dC9wbGFpbicsJ2F0dGFjaG1lbnQucGhwJyk7DQokbWV0aG9kVmFs
dWUgPSBuZXcgU09BUF9WYWx1ZSgndGVzdGF0dGFjaCcsICdTdHJ1Y3QnLCBhcnJheSgkdikpOw0K
DQovLyBzZWUgdGhlIG1pbWUgYXJyYXkNCi8vJHZhbCA9ICRzb2FwX2Jhc2UtPl9tYWtlRW52ZWxv
cGUoJG1ldGhvZFZhbHVlKTsNCi8vcHJpbnRfcigkdmFsKTsNCg0KJGNsaWVudCA9IG5ldyBTT0FQ
X0NsaWVudCgnaHR0cDovL2xvY2FsaG9zdC9zb2FwX2ludGVyb3Avc2VydmVyX3JvdW5kMi5waHAn
KTsNCiRyZXNwID0gJGNsaWVudC0+Y2FsbCgnZWNob01pbWVBdHRhY2htZW50JyxhcnJheSgkdikp
Ow0KcHJpbnRfcigkcmVzcCk7DQpwcmludCAkY2xpZW50LT53aXJlOw0KPz4=
--=_d624611fe466a88d956a205651c74fdb--';

$_SERVER['CONTENT_TYPE'] = 'multipart/related; type=text/xml; boundary="=_a2cbb051424cc43e72d3c8c8d0b8f70e"';
$test='--=_a2cbb051424cc43e72d3c8c8d0b8f70e
Content-Type: text/xml; charset="UTF-8"

<?xml version="1.0" encoding="UTF-8"?>

<SOAP-ENV:Envelope  xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
  xmlns:xsd="http://www.w3.org/2001/XMLSchema"
  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
  xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/"
  SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
<SOAP-ENV:Body>

<echoMimeAttachment>
<test href="cid:a223fea3c35b5f0e6dedf8da75efd6b3"/></echoMimeAttachment>
</SOAP-ENV:Body>
</SOAP-ENV:Envelope>

--=_a2cbb051424cc43e72d3c8c8d0b8f70e
Content-Disposition: attachment.php
Content-Type: text/plain
Content-Transfer-Encoding: base64
Content-ID: <a223fea3c35b5f0e6dedf8da75efd6b3>

PD9waHANCnJlcXVpcmVfb25jZSgiU09BUC9DbGllbnQucGhwIik7DQpyZXF1aXJlX29uY2UoIlNP
QVAvdGVzdC90ZXN0LnV0aWxpdHkucGhwIik7DQpyZXF1aXJlX29uY2UoIlNPQVAvVmFsdWUucGhw
Iik7DQokc29hcF9iYXNlID0gbmV3IFNPQVBfQmFzZSgpOw0KDQokdiA9ICBuZXcgU09BUF9BdHRh
Y2htZW50KCd0ZXN0JywndGV4dC9wbGFpbicsJ2F0dGFjaG1lbnQucGhwJyk7DQokbWV0aG9kVmFs
dWUgPSBuZXcgU09BUF9WYWx1ZSgndGVzdGF0dGFjaCcsICdTdHJ1Y3QnLCBhcnJheSgkdikpOw0K
DQovLyBzZWUgdGhlIG1pbWUgYXJyYXkNCi8vJHZhbCA9ICRzb2FwX2Jhc2UtPl9tYWtlRW52ZWxv
cGUoJG1ldGhvZFZhbHVlKTsNCi8vcHJpbnRfcigkdmFsKTsNCg0KJGNsaWVudCA9IG5ldyBTT0FQ
X0NsaWVudCgnaHR0cDovL2xvY2FsaG9zdC9zb2FwX2ludGVyb3Avc2VydmVyX3JvdW5kMi5waHAn
KTsNCiRyZXNwID0gJGNsaWVudC0+Y2FsbCgnZWNob01pbWVBdHRhY2htZW50JyxhcnJheSgkdikp
Ow0KI3ByaW50X3IoJHJlc3ApOw0KcHJpbnQgJGNsaWVudC0+d2lyZTsNCj8+
--=_a2cbb051424cc43e72d3c8c8d0b8f70e--
';
$server->service($test, '',TRUE);
print $server->response;
?>