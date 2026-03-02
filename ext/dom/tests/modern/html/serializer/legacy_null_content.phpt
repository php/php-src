--TEST--
Serialize legacy nodes with NULL content
--EXTENSIONS--
dom
--FILE--
<?php
$dom = Dom\HTMLDocument::createEmpty();
$root = $dom->appendChild($dom->createElement('html'));

$root->appendChild($dom->importLegacyNode(new DOMText));
$root->appendChild($dom->importLegacyNode(new DOMComment));
$root->appendChild($dom->importLegacyNode(new DOMProcessingInstruction('target')));
$root->appendChild($dom->importLegacyNode(new DOMCdataSection('')));

echo $dom->saveHTML(), "\n";
echo $dom->documentElement->innerHTML, "\n";
?>
--EXPECT--
<html><!----><?target ></html>
<!----><?target >
