--TEST--
Serialize modern document with LIBXML_NOXMLDECL
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createFromString('<?xml version="1.0"?><root/>');
echo $dom->saveXml(options: LIBXML_NOXMLDECL), "\n";

?>
--EXPECT--
<root/>
