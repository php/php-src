--TEST--
DOMDocumentType::systemId with empty value.
--CREDITS--
Eric Lee Stewart <ericleestewart@gmail.com>
# TestFest Atlanta 2009-05-25
--EXTENSIONS--
dom
--FILE--
<?php
$xml  = '<?xml version="1.0" encoding="UTF-8" ?>';
$xml .= '<!DOCTYPE chapter>';
$xml .= '<chapter>1</chapter>';
$doc = new DOMDocument();
$doc->loadXML($xml);
$doctype = $doc->doctype;
var_dump($doctype->systemId);
?>
--EXPECT--
string(0) ""
