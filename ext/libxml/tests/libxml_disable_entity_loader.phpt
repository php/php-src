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

function parseXML($xml) {
  $doc = new DOMDocument();
  $doc->resolveExternals = true;
  $doc->substituteEntities = true;
  $doc->validateOnParse = false;
  $doc->loadXML($xml, 0);
  return $doc->saveXML();
}

var_dump(strpos(parseXML($xml), 'SECRET_DATA') !== false);
var_dump(libxml_disable_entity_loader(true));
var_dump(strpos(parseXML($xml), 'SECRET_DATA') === false);

echo "Done\n";
?>
--EXPECTF--
bool(true)

Deprecated: Function libxml_disable_entity_loader() is deprecated in %s on line %d
bool(false)

Warning: DOMDocument::loadXML(): I/O warning : failed to load external entity "%s" in %s on line %d

Warning: DOMDocument::loadXML(): Failure to process entity xxe in Entity, line: %d in %s on line %d

Warning: DOMDocument::loadXML(): Entity 'xxe' not defined in Entity, line: %d in %s on line %d
bool(true)
Done
