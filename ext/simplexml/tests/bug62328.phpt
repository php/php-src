--TEST--
Bug #62328 (implementing __toString and a cast to string fails)
--EXTENSIONS--
simplexml
--FILE--
<?php
class UberSimpleXML extends SimpleXMLElement {
    public function __toString() {
        return 'stringification';
    }
}

$xml = new UberSimpleXML('<xml/>');

var_dump((string) $xml);
var_dump($xml->__toString());
?>
--EXPECT--
string(15) "stringification"
string(15) "stringification"
