--TEST--
doXInclude
--EXTENSIONS--
xsl
dom
--FILE--
<?php

chdir(__DIR__);

$xml = new DOMDocument;
$xml->loadXML('<?xml version="1.0"?><root/>');

$xsl = new DOMDocument;
$xsl->loadXML(<<<XML
<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
    <xsl:template match="/root">
        <container>
            <xsl:value-of select="document('data.xml')/data/content"/>
        </container>
    </xsl:template>
</xsl:stylesheet>
XML);

$xslt = new XSLTProcessor;
$xslt->doXInclude = true;
$xslt->importStylesheet($xsl);
echo $xslt->transformToXml($xml);

$xslt = new XSLTProcessor;
$xslt->doXInclude = false;
$xslt->importStylesheet($xsl);
echo $xslt->transformToXml($xml);

?>
--EXPECT--
<?xml version="1.0"?>
<container>This is sample content</container>
<?xml version="1.0"?>
<container/>
