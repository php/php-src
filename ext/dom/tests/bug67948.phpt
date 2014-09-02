--TEST--
Test for feature #67948: DOMDocument::saveHTML() should accept a DOMNodeList for the $node argument
--CREDITS--
Boro Sitnikovski <buritomath@yahoo.com>
--FILE--
<?php
$html = <<<HTML
<div>
<span>Some random data</span>
</div>
HTML;

$xml = <<<XML
<div>
<span>Some random data</span>
</div>
XML;

$doc = new DOMDocument;
$doc->loadHTML($html);
$div = $doc->getElementsByTagName('div')->item(0);
$div->appendChild($doc->createElement('span', 'more data'));
$data = $doc->saveHTML($div->childNodes);
var_dump($data);

$doc = new DOMDocument;
$doc->loadXML($xml);
$div = $doc->getElementsByTagName('div')->item(0);
$div->appendChild($doc->createElement('span', 'more data'));
$data = $doc->saveXML($div->childNodes);
var_dump($data);
?>
--EXPECT--
string(51) "<span>Some random data</span><span>more data</span>"
string(51) "<span>Some random data</span><span>more data</span>"
