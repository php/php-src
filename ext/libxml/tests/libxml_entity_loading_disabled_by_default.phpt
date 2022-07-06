--TEST--
libxml_disable_entity_loader()
--SKIPIF--
<?php
if (!extension_loaded('libxml')) die('skip libxml extension not available');
if (!extension_loaded('dom')) die('skip dom extension not available');
--FILE--
<?php

$xml = <<<EOT
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE test [<!ENTITY xxe SYSTEM "XXE_URI">]>
<foo>&xxe;</foo>
EOT;

$dir = str_replace('\\', '/', __DIR__);
$xml = str_replace('XXE_URI', $dir . '/libxml_disable_entity_loader_payload.txt', $xml);

function parseXML1($xml) {
  $doc = new DOMDocument();
  $doc->loadXML($xml, 0);
  return $doc->saveXML();
}

function parseXML2($xml) {
  return simplexml_load_string($xml);
}

function parseXML3($xml) {
  $p = xml_parser_create();
  xml_parse_into_struct($p, $xml, $vals, $index);
  xml_parser_free($p);
  return var_export($vals, true);
}

function parseXML4($xml) {
  // This is the only time we enable external entity loading.
  return simplexml_load_string($xml, 'SimpleXMLElement', LIBXML_NOENT);
}

var_dump(strpos(parseXML1($xml), 'SECRET_DATA') === false);
var_dump(strpos(parseXML2($xml), 'SECRET_DATA') === false);
var_dump(strpos(parseXML3($xml), 'SECRET_DATA') === false);
var_dump(strpos(parseXML4($xml), 'SECRET_DATA') === false);

echo "Done\n";
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(false)
Done
