--TEST--
SOAP Server 31: Handling classes which implements Iterator
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
class ItemArray implements Iterator {
	private $a = array();

	public function __construct(array $a) {
		$this->a = $a;
	}

    public function rewind()    { return reset($this->a); }
    public function current()   { return current($this->a); }
    public function key()       { return key($this->a); }
    public function next()      { return next($this->a); }
    public function valid()     { return (current($this->a) !== false); }
}

class Item {
    public $text;

	public function __construct($n) {
		$this->text = 'text'.$n;
	}
}

class handlerClass {
    public function getItems()
    {
        return new ItemArray(array(
        		new Item(0),
        		new Item(1),
        		new Item(2),
        		new Item(3),
        		new Item(4),
        		new Item(5),
        		new Item(6),
        		new Item(7),
        		new Item(8),
        		new Item(9)
        	));
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
