--TEST--
SimpleXMLElement: Test to ensure that the required attribute name correctly is giving a warning
--CREDITS--
Havard Eide <nucleuz@gmail.com>
#PHPTestFest2009 Norway 2009-06-09 \o/
--EXTENSIONS--
simplexml
--FILE--
<?php
$a = new SimpleXMLElement("<php>testfest</php>");

try {
    $a->addAttribute( "", "" );
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

echo $a->asXML();
?>
--EXPECT--
SimpleXMLElement::addAttribute(): Argument #1 ($qualifiedName) cannot be empty
<?xml version="1.0"?>
<php>testfest</php>
