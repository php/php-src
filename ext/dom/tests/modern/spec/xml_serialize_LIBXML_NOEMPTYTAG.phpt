--TEST--
XML Serialization with LIBXML_NOEMPTYTAG
--EXTENSIONS--
dom
--FILE--
<?php

$doc = DOM\XMLDocument::createFromString('<root><child/><br xmlns="http://www.w3.org/1999/xhtml"/></root>');
echo $doc->saveXML($doc, LIBXML_NOEMPTYTAG), "\n";

$doc->formatOutput = true;
echo $doc->saveXML($doc, LIBXML_NOEMPTYTAG), "\n";

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<root><child></child><br xmlns="http://www.w3.org/1999/xhtml"></br></root>
<?xml version="1.0" encoding="UTF-8"?>
<root>
  <child></child>
  <br xmlns="http://www.w3.org/1999/xhtml"></br>
</root>
