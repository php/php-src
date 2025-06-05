--TEST--
XSLTProcessor::transformToStream() function - errors
--EXTENSIONS--
xsl
--FILE--
<?php
include("prepare.inc");
$proc->importStylesheet($xsl);

$stream = fopen('php://output', 'w');
try {
    $proc->transformToStream($dom, $stream, 'nope');
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
fclose($stream);
?>
--EXPECT--
XSLTProcessor::transformToStream(): Argument #3 ($encoding) is not a valid document encoding
