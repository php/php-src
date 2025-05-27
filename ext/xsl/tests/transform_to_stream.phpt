--TEST--
XSLTProcessor::transformToStream() function - normal
--EXTENSIONS--
xsl
--FILE--
<?php
include("prepare.inc");
$proc->importStylesheet($xsl);

$stream = fopen('php://output', 'w');
$written = $proc->transformToStream($dom, $stream);
fclose($stream);

echo "\n";
var_dump($written);

$stream = fopen('php://output', 'w');
$written = $proc->transformToStream($dom, $stream, 'iso-8859-1');
fclose($stream);

echo "\n";
var_dump($written);
?>
--EXPECT--
<?xml version="1.0" encoding="iso-8859-1"?>
<html><body>bar
a1 b1 c1 <br/> 
a2 c2 <br/> 
Ã¤3 b3 c3 <br/> 
</body></html>
int(120)
<?xml version="1.0" encoding="iso-8859-1"?>
<html><body>bar
a1 b1 c1 <br/> 
a2 c2 <br/> 
ä3 b3 c3 <br/> 
</body></html>
int(119)
