--TEST--
Dom\XMLDocument: Dom\Notation publicId and systemId properties
--EXTENSIONS--
dom
--FILE--
<?php
$xml = <<<'XML'
<!DOCTYPE r [
    <!NOTATION image PUBLIC "-//Example//EN" "image.dat">
    <!ELEMENT r EMPTY>
]>
<r/>
XML;

$doc = Dom\XMLDocument::createFromString($xml);
$notation = $doc->doctype->notations->getNamedItem('image');

var_dump($notation->publicId);
var_dump($notation->systemId);
?>
--EXPECT--
string(14) "-//Example//EN"
string(9) "image.dat"
