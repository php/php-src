--TEST--
XSLTProcessor namespace mapper lifetime
--EXTENSIONS--
dom
xsl
--FILE--
<?php

$input = DOM\XMLDocument::createFromString(<<<XML
<root>
    <hello>World</hello>
</root>
XML);

$xslXML = <<<XML
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:php="http://php.net/xsl"
    xmlns:xsdl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="text" omit-xml-declaration="yes" indent="no"/>
    <xsl:template match="/root">
        <xsl:value-of select="php:function('strtoupper', string(./hello))"/>
    </xsl:template>
</xsl:stylesheet>
XML;

$processor = new XSLTProcessor();
// The fact that this is a temporary is important!
// And yes, this is done twice on purpose to check for leaks
$processor->importStylesheet(DOM\XMLDocument::createFromString($xslXML));
$processor->importStylesheet(DOM\XMLDocument::createFromString($xslXML));
$processor->registerPHPFunctions();
echo $processor->transformToXML($input), "\n";

?>
--EXPECT--
WORLD
