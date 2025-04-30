--TEST--
Request #71571 (XSLT processor should provide option to change maxDepth) - variant A
--EXTENSIONS--
xsl
--INI--
error_reporting=E_ALL
--FILE--
<?php

$myxsl = <<<'EOF'
<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:template match="/">
        <xsl:call-template name="recurse"/>
    </xsl:template>

    <xsl:template name="recurse">
        <xsl:call-template name="recurse"/>
    </xsl:template>
</xsl:stylesheet>
EOF;

$xsl = new DOMDocument();
$xsl->loadXML($myxsl);

$doc = new DOMDocument();

$proc = new XSLTProcessor;
$proc->maxTemplateDepth = 2;
$proc->importStyleSheet($xsl);
$proc->transformToDoc($doc);

?>
--EXPECTF--
Warning: XSLTProcessor::transformToDoc(): runtime error: file %s line 8 element call-template in %s on line %d

Warning: XSLTProcessor::transformToDoc(): xsltApplySequenceConstructor: A potential infinite template recursion was detected.
You can adjust $maxTemplateDepth in order to raise the maximum number of nested template calls and variables/params (currently set to 2). in %s on line %d

Warning: XSLTProcessor::transformToDoc(): Templates: in %s on line %d

Warning: XSLTProcessor::transformToDoc(): #0 name recurse  in %s on line %d

Warning: XSLTProcessor::transformToDoc(): #1 name recurse  in %s on line %d

Warning: XSLTProcessor::transformToDoc(): #2 name /  in %s on line %d

Warning: XSLTProcessor::transformToDoc(): Variables: in %s on line %d
