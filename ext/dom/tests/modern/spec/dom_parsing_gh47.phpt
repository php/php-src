--TEST--
DOM-Parsing GH-47 (XML null namespaces need to be preserved)
--EXTENSIONS--
dom
--FILE--
<?php

$document = DOM\XMLDocument::createFromString(<<<XML
<!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <people xmlns="">
      <person>
        <lastname>Smith</lastname>
        <firstname>Joe</firstname>
      </person>
      <person>
        <lastname>Jones</lastname>
        <firstname>John</firstname>
      </person>
    </people>
  </head>
</html>
XML);

echo $document->saveXML(), "\n";

$people = $document->getElementsByTagNameNS(null, 'people')->item(0);
echo $document->saveXML($people), "\n";

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <people xmlns="">
      <person>
        <lastname>Smith</lastname>
        <firstname>Joe</firstname>
      </person>
      <person>
        <lastname>Jones</lastname>
        <firstname>John</firstname>
      </person>
    </people>
  </head>
</html>
<people xmlns="">
      <person>
        <lastname>Smith</lastname>
        <firstname>Joe</firstname>
      </person>
      <person>
        <lastname>Jones</lastname>
        <firstname>John</firstname>
      </person>
    </people>
