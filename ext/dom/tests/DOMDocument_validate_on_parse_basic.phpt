--TEST--
DOMDocument::$validateOnParse - read/write tests (dom_document_validate_on_parse_read/dom_document_validate_on_parse_write)
--CREDITS--
Hans Zaunere
# TestFest 2009 NYPHP
--EXTENSIONS--
dom
--FILE--
<?php

require_once('dom_test.inc');

$dom = new DOMDocument;
$dom->loadXML($xmlstr);

if( !$dom )
{
    echo "Error while parsing the document\n";
    exit;
}

echo "Checking documented default value: ";
var_dump($dom->validateOnParse);

$dom->validateOnParse = TRUE;
echo "Setting validateOnParse to TRUE: ";
var_dump($dom->validateOnParse);

$dom->validateOnParse = FALSE;
echo "Setting validateOnParse to FALSE: ";
var_dump($dom->validateOnParse);

?>
--EXPECT--
Checking documented default value: bool(false)
Setting validateOnParse to TRUE: bool(true)
Setting validateOnParse to FALSE: bool(false)
