--TEST--
Test 5: HTML Test
--SKIPIF--
<?php  require_once('skipif.inc'); ?>
--FILE--
<?php
$dom = new domdocument;
$dom->loadHTMLFile(dirname(__FILE__)."/test.html");
print  "--- save as XML\n";

print $dom->saveXML();
print  "--- save as HTML\n";

print $dom->saveHTML();
--EXPECT--
--- save as XML
<?xml version="1.0" standalone="yes"?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" "http://www.w3.org/TR/REC-html40/loose.dtd">
<html><head><title>Hello world</title></head><body><p>
This is a not well-formed<br/>
html files with undeclared entities 
</p></body></html>
--- save as HTML
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" "http://www.w3.org/TR/REC-html40/loose.dtd">
<html>
<head><title>Hello world</title></head>
<body><p>
This is a not well-formed<br>
html files with undeclared entities&nbsp;
</p></body>
</html>
