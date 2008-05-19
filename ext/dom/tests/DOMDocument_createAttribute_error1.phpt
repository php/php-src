--TEST--
DomDocument::createAttribute() - error test for DomDocument::createAttribute()
--CREDITS--
Muhammad Khalid Adnan
# TestFest 2008
--FILE--
<?php

$doc = new DOMDocument;

$node = $doc->createElement("para");
$newnode = $doc->appendChild($node);

try {
    $failed_test_attribute = $doc->createAttribute("ha haha");
    $node->appendChild($failed_test_attribute);

    echo $doc->saveXML();
}
catch (DOMException $e) {
    echo 'Test failed!', PHP_EOL;
}

?>
--EXPECT--
Test failed!

