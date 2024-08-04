--TEST--
Serialize XML attribute
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createFromString("<root/>");
$root = $dom->documentElement;
$root->setAttribute("foo", "<>\"& \t\n\rfoo   ");

echo $dom->saveXml(), "\n";

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<root foo="&lt;&gt;&quot;&amp; &#9;&#10;&#13;foo   "/>
