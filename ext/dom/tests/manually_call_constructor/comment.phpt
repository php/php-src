--TEST--
Manually call __construct() - comment variation
--EXTENSIONS--
dom
--FILE--
<?php

$comment = new DOMComment("my value");
var_dump($comment->nodeName, $comment->nodeValue);
$comment->__construct("my new value");
var_dump($comment->nodeName, $comment->nodeValue);

$doc = new DOMDocument();
$doc->loadXML(<<<XML
<?xml version="1.0"?>
<container/>
XML);
$doc->documentElement->appendChild($comment);
echo $doc->saveXML();

$comment->__construct("my even newer value");
$doc->documentElement->appendChild($comment);
echo $doc->saveXML();

?>
--EXPECTF--
string(8) "#comment"
string(8) "my value"

Fatal error: Uncaught Error: Couldn't fetch DOMComment. Node no longer exists in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
