--TEST--
GH-12616 (DOM: Removing XMLNS namespace node results in invalid default: prefix)
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument();
$doc->loadXML(
    <<<XML
    <container xmlns="http://symfony.com/schema/dic/services">
      CHILDREN
    </container>
    XML
);

$doc->documentElement->removeAttributeNS('http://symfony.com/schema/dic/services', '');
echo $doc->saveXML();

$new = new DOMDocument();
$new->append(
    $new->importNode($doc->documentElement, true)
);

echo $new->saveXML();

?>
--EXPECT--
<?xml version="1.0"?>
<container>
  CHILDREN
</container>
<?xml version="1.0"?>
<container>
  CHILDREN
</container>
