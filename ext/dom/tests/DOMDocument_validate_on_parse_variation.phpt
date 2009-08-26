--TEST--
DOMDocument::$validateOnParse - effectual determination (dom_document_validate_on_parse_read/dom_document_validate_on_parse_write)
--CREDITS--
Hans Zaunere
# TestFest 2009 NYPHP
--SKIPIF--
<?php
require_once('skipif.inc');

// need external DTD/XML docs
if( @file_get_contents('http://www.php.net/') === FALSE )
    exit('skip network not available');
?>
--FILE--
<?php

require_once('dom_test.inc');

$XMLStringGood = file_get_contents('http://www.php.net/');

$dom = new DOMDocument;
$dom->resolveExternals = TRUE;

$dom->validateOnParse = FALSE;
echo "validateOnParse set to FALSE: \n";
$dom->loadXML($XMLStringGood);
echo "No Error Report Above\n";

$BogusElement = $dom->createElement('NYPHP','DOMinatrix');
$Body = $dom->getElementsByTagName('body')->item(0);
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

Warning: DOMDocument::loadXML(): Element body content does not follow the DTD, expecting (p | h1 | h2 | h3 | h4 | h5 | h6 | div | ul | ol | dl | pre | hr | blockquote | address | fieldset | table | form | noscript | ins | del | script)*, got (div div div div div NYPHP) in Entity, line: %d in %s on line %d
Error Report Above

