--TEST--
DOMDocument::$validateOnParse - effectual determination (dom_document_validate_on_parse_read/dom_document_validate_on_parse_write)
--CREDITS--
Hans Zaunere
# TestFest 2009 NYPHP
--SKIPIF--
<?php
require_once('skipif.inc');
?>
--FILE--
<?php

require_once('dom_test.inc');

chdir(__DIR__);
$XMLStringGood = file_get_contents(dirname(__FILE__).'/note.xml');

$dom = new DOMDocument;
$dom->resolveExternals = TRUE;

$dom->validateOnParse = FALSE;
echo "validateOnParse set to FALSE: \n";
$dom->loadXML($XMLStringGood);
echo "No Error Report Above\n";

$BogusElement = $dom->createElement('NYPHP','DOMinatrix');
$Body = $dom->getElementsByTagName('from')->item(0);
$Body->appendChild($BogusElement);
$XMLStringBad = $dom->saveXML();

echo "validateOnParse set to TRUE: \n";
$dom->validateOnParse = TRUE;
$dom->loadXML($XMLStringBad);
echo "Error Report Above\n";

?>
--EXPECTF--
validateOnParse set to FALSE: 
No Error Report Above
validateOnParse set to TRUE: 

Warning: DOMDocument::loadXML(): No declaration for element NYPHP in Entity, line: %d in %s on line %d

Warning: DOMDocument::loadXML(): Element from was declared #PCDATA but contains non text nodes in Entity, line: %d in %s on line %d
Error Report Above

