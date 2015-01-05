--TEST--
SOAP Server 30: Handling classes which extend the SPL ArrayObject or ArrayIterator as arrays in wsdl mode
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
class ItemArray extends ArrayObject {

}

class Item {
    public $text;
}

class handlerClass {
    public function getItems()
    {
        $items = new ItemArray(array());

        for ($i = 0; $i < 10; $i++) {
            $item = new Item();
            $item->text = 'text'.$i;

            $items[] = $item;
        }

        return $items;
    }
}

$server = new SoapServer(dirname(__FILE__)."/server030.wsdl");
$server->setClass('handlerClass');

$HTTP_RAW_POST_DATA = <<<EOF
<?xml version="1.0" encoding="ISO-8859-1"?>
<SOAP-ENV:Envelope
  SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"
  xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/">
  <SOAP-ENV:Body>
    <getItems/>
  </SOAP-ENV:Body>
</SOAP-ENV:Envelope>
EOF;

$server->handle($HTTP_RAW_POST_DATA);
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://testuri.org" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:getItemsResponse><getItemsReturn SOAP-ENC:arrayType="ns1:Item[10]" xsi:type="ns1:ItemArray"><item xsi:type="ns1:Item"><text xsi:type="xsd:string">text0</text></item><item xsi:type="ns1:Item"><text xsi:type="xsd:string">text1</text></item><item xsi:type="ns1:Item"><text xsi:type="xsd:string">text2</text></item><item xsi:type="ns1:Item"><text xsi:type="xsd:string">text3</text></item><item xsi:type="ns1:Item"><text xsi:type="xsd:string">text4</text></item><item xsi:type="ns1:Item"><text xsi:type="xsd:string">text5</text></item><item xsi:type="ns1:Item"><text xsi:type="xsd:string">text6</text></item><item xsi:type="ns1:Item"><text xsi:type="xsd:string">text7</text></item><item xsi:type="ns1:Item"><text xsi:type="xsd:string">text8</text></item><item xsi:type="ns1:Item"><text xsi:type="xsd:string">text9</text></item></getItemsReturn></ns1:getItemsResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
