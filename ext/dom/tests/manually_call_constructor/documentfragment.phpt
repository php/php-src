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
--EXPECTF--
string(0) ""

Fatal error: Uncaught Error: Couldn't fetch DOMDocumentFragment. Node no longer exists in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
