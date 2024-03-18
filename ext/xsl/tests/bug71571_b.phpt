--TEST--
Request #71571 (XSLT processor should provide option to change maxDepth) - variant B
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
        <xsl:param name="COUNT">1</xsl:param>
        <xsl:call-template name="recurse">
            <xsl:with-param name="COUNT" select="$COUNT + 1"/>
        </xsl:call-template>
    </xsl:template>
</xsl:stylesheet>
EOF;

$xsl = new DOMDocument();
$xsl->loadXML($myxsl);

$doc = new DOMDocument();

$proc = new XSLTProcessor;
// Set the template depth limit so high that we will certainly hit the variable depth limit first.
$proc->maxTemplateDepth = 2**30;
$proc->maxTemplateVars = 2;
$proc->importStyleSheet($xsl);
$proc->transformToDoc($doc);

?>
--EXPECTF--
Warning: XSLTProcessor::transformToDoc(): runtime error: file %s line 8 element param in %s on line %d

Warning: XSLTProcessor::transformToDoc(): xsltApplyXSLTTemplate: A potential infinite template recursion was detected.
You can adjust $maxTemplateVars in order to raise the maximum number of variables/params (currently set to 2). in %s on line %d

Warning: XSLTProcessor::transformToDoc(): Templates: in %s on line %d

Warning: XSLTProcessor::transformToDoc(): #0 name recurse  in %s on line %d

Warning: XSLTProcessor::transformToDoc(): #1 name recurse  in %s on line %d

Warning: XSLTProcessor::transformToDoc(): #2 name /  in %s on line %d

Warning: XSLTProcessor::transformToDoc(): Variables: in %s on line %d

Warning: XSLTProcessor::transformToDoc(): #0 in %s on line %d

Warning: XSLTProcessor::transformToDoc(): COUNT  in %s on line %d

Warning: XSLTProcessor::transformToDoc(): #1 in %s on line %d

Warning: XSLTProcessor::transformToDoc(): param COUNT  in %s on line %d
