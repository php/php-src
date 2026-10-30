--TEST--
DOMDocument::getElementsByTagName() liveness with DOMDocument::xinclude()
--EXTENSIONS--
dom
--FILE--
<?php

// Adapted from https://www.php.net/manual/en/domdocument.xinclude.php
$xml = <<<EOD
<?xml version="1.0" ?>
<chapter xmlns:xi="http://www.w3.org/2001/XInclude">
 <p>Hello</p>
 <para>
  <xi:include href="book.xml">
   <xi:fallback>
    <p>xinclude: book.xml not found</p>
   </xi:fallback>
  </xi:include>
 </para>
</chapter>
EOD;

$dom = new DOMDocument;
$dom->loadXML($xml);
$elements = $dom->getElementsByTagName('p');
var_dump($elements->item(0)->textContent);
@$dom->xinclude();
var_dump($elements->item(1)->textContent);
echo $dom->saveXML();

?>
--EXPECT--
string(5) "Hello"
string(28) "xinclude: book.xml not found"
<?xml version="1.0"?>
<chapter xmlns:xi="http://www.w3.org/2001/XInclude">
 <p>Hello</p>
 <para>
  
    <p>xinclude: book.xml not found</p>
   
 </para>
</chapter>
