--TEST--
Bug #70078 (XSL callbacks with nodes as parameter leak memory)
--EXTENSIONS--
xsl
--FILE--
<?php
// create big dummy document:
$dom = new \DOMDocument();
$rootNode = $dom->appendChild($dom->createElement('root'));
for ($i = 0; $i <= 100; $i++) {
    $level1Node = $rootNode->appendChild($dom->createElement('level1'));
    for ($j = 0; $j <= 100; $j++) {
        $level2Node = $level1Node->appendChild($dom->createElement('level2'));
        for ($k = 0; $k <= 10; $k++) {
            $level3Node = $level2Node->appendChild($dom->createElement('level3', 'test'));
        }
    }
}

function testPhpFunction($node) {
    return 'test2';
}

$xslStr = <<<EOF
<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:php="http://php.net/xsl">
    <xsl:template match="root">
        <output>
            <xsl:for-each select="level1">
                <node>
                    <xsl:value-of select="php:function('testPhpFunction', .)" />
                </node>
            </xsl:for-each>
        </output>
    </xsl:template>
</xsl:stylesheet>
EOF;

$xsl = new \DOMDocument();
$xsl->loadXML($xslStr);
$xslt = new \XSLTProcessor();
$xslt->registerPHPFunctions('testPhpFunction');
$xslt->importStyleSheet($xsl);

echo $xslt->transformToXML($dom);
?>
--EXPECT--
<?xml version="1.0"?>
<output xmlns:php="http://php.net/xsl"><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node><node>test2</node></output>
