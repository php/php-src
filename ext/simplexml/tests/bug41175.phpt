--TEST--
Bug #41175 (addAttribute() fails to add an attribute with an empty value)
--FILE--
<?php

$xml = new SimpleXmlElement("<img></img>");
$xml->addAttribute("src", "foo");
$xml->addAttribute("alt", "");
echo $xml->asXML();

echo "Done\n";
?>
--EXPECT--	
<?xml version="1.0"?>
<img src="foo" alt=""/>
Done