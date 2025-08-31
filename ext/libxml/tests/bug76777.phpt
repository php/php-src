--TEST--
Bug #76777 (first parameter of libxml_set_external_entity_loader callback undefined)
--EXTENSIONS--
libxml
dom
--SKIPIF--
<?php
if (getenv("SKIP_ONLINE_TESTS")) die('skip online test');
?>
--FILE--
<?php
$xml=<<<EOF
<?xml version="1.0"?>
<test/>
EOF;

$xsd=<<<EOF
<?xml version="1.0"?>
<xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema">
  <xs:include schemaLocation="nonexistent.xsd"/>
  <xs:element name="test"/>
</xs:schema>
EOF;

libxml_set_external_entity_loader(function($p,$s,$c) {
    var_dump($p,$s,$c);
    die();
});

$dom=new DOMDocument($xml);
$dom->schemaValidateSource($xsd);
?>
--EXPECT--
NULL
string(15) "nonexistent.xsd"
array(4) {
  ["directory"]=>
  NULL
  ["intSubName"]=>
  NULL
  ["extSubURI"]=>
  NULL
  ["extSubSystem"]=>
  NULL
}
