--TEST--
Bug #54382 DOMNode::getAttributeNodeNS doesn't get xmlns* attributes
--SKIPIF--
<?php
require_once('skipif.inc');
?>
--FILE--
<?php
$xmlString = '<?xml version="1.0" encoding="utf-8" ?>
<root xmlns="http://ns" xmlns:ns2="http://ns2">
    <ns2:child />
</root>';

$xml=new DOMDocument();
$xml->loadXML($xmlString);
$de = $xml->documentElement;

$ns2 = $de->getAttributeNodeNS("http://www.w3.org/2000/xmlns/", "ns2");
if ($ns2 == NULL) {
  echo 'namespace node does not exist.' . "\n";
} else {
  echo 'namespace node prefix=' . $ns2->prefix . "\n";
  echo 'namespace node namespaceURI=' . $ns2->namespaceURI . "\n";
}
--EXPECT--
namespace node prefix=ns2
namespace node namespaceURI=http://ns2
