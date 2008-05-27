--TEST--
SimpleXML [profile]: Accessing namespaced root and non namespaced children
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php

$xml =b<<<EOF
<?xml version="1.0" encoding="utf-8"?>
<soap:Envelope
xmlns:soap="http://schemas.xmlsoap.org/soap/envelope/" 
xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
xmlns:xsd="http://www.w3.org/2001/XMLSchema"
>
<soap:Body>
<businessList foo="bar">
<businessInfo businessKey="bla"/>
</businessList>
</soap:Body> 
</soap:Envelope>
EOF;

$sxe = simplexml_load_string($xml);
$nsl = $sxe->getNamespaces();
var_dump($nsl);

$sxe = simplexml_load_string($xml, NULL, 0, $nsl['soap']);
var_dump($sxe->Body);
var_dump($sxe->Body->children(''));
var_dump($sxe->Body->children('')->businessList);

?>
===DONE===
--EXPECTF--
array(1) {
  [u"soap"]=>
  unicode(41) "http://schemas.xmlsoap.org/soap/envelope/"
}
object(SimpleXMLElement)#%s (0) {
}
object(SimpleXMLElement)#%s (1) {
  [u"businessList"]=>
  object(SimpleXMLElement)#%s (2) {
    [u"@attributes"]=>
    array(1) {
      [u"foo"]=>
      unicode(3) "bar"
    }
    [u"businessInfo"]=>
    object(SimpleXMLElement)#%s (1) {
      [u"@attributes"]=>
      array(1) {
        [u"businessKey"]=>
        unicode(3) "bla"
      }
    }
  }
}
object(SimpleXMLElement)#%s (2) {
  [u"@attributes"]=>
  array(1) {
    [u"foo"]=>
    unicode(3) "bar"
  }
  [u"businessInfo"]=>
  object(SimpleXMLElement)#%s (1) {
    [u"@attributes"]=>
    array(1) {
      [u"businessKey"]=>
      unicode(3) "bla"
    }
  }
}
===DONE===
