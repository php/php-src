--TEST--
JSON encoding a DOMDocument
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument;
echo json_encode($doc);
?>
--EXPECT--
{}
