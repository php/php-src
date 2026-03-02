--TEST--
Manually call __construct() - document variation
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument();
$doc->loadXML('<?xml version="1.0"?><foo/>');
$doc->__construct("1.1", "UTF-8");
echo $doc->saveXML();

?>
--EXPECT--
<?xml version="1.1" encoding="UTF-8"?>
