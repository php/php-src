--TEST--
DOMDocument::saveHTMLFile() should format output on demand
--CREDITS--
Knut Urdalen <knut@php.net>
#PHPTestFest2009 Norway 2009-06-09 \o/
--EXTENSIONS--
dom
--FILE--
<?php
$filename = __DIR__."/DOMDocument_saveHTMLFile_formatOutput.html";
$doc = new DOMDocument('1.0');
$doc->formatOutput = true;
$root = $doc->createElement('html');
$root = $doc->appendChild($root);
$head = $doc->createElement('head');
$head = $root->appendChild($head);
$title = $doc->createElement('title');
$title = $head->appendChild($title);
$text = $doc->createTextNode('This is the title');
$text = $title->appendChild($text);
$bytes = $doc->saveHTMLFile($filename);
var_dump($bytes);
echo file_get_contents($filename);
unlink($filename);
?>
--EXPECT--
int(129)
<html><head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>This is the title</title>
</head></html>
