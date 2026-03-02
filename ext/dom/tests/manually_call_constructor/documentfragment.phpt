--TEST--
Manually call __construct() - document fragment variation
--EXTENSIONS--
dom
--FILE--
<?php

$fragment = new DOMDocumentFragment();
var_dump($fragment->textContent);
$fragment->__construct();
var_dump($fragment->textContent);

$doc = new DOMDocument();
$doc->loadXML(<<<XML
<?xml version="1.0"?>
<container/>
XML);
@$doc->documentElement->appendChild($fragment);
echo $doc->saveXML();

$fragment->__construct();
@$doc->documentElement->appendChild($fragment);
echo $doc->saveXML();

?>
--EXPECT--
string(0) ""
string(0) ""
<?xml version="1.0"?>
<container/>
<?xml version="1.0"?>
<container/>
