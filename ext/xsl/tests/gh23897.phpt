--TEST--
GH-23897 (Assertion failure in php_dom_xpath_callback_dispatch() after failed registerPHPFunctions())
--EXTENSIONS--
xsl
--CREDITS--
YuanchengJiang
--FILE--
<?php
$dom = new DOMDocument();
$dom->loadXML('<root><level1/></root>');

$xsl = new DOMDocument();
$xsl->loadXML(<<<XML
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:php="http://php.net/xsl">
<xsl:template match="root">
<xsl:value-of select="php:function('testPhpFunction', .)" />
</xsl:template>
</xsl:stylesheet>
XML);

$xslt = new XSLTProcessor();
try {
    $xslt->registerPHPFunctions('testPhpFunction');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
$xslt->importStylesheet($xsl);
try {
    var_dump($xslt->transformToXml($dom));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
TypeError: XSLTProcessor::registerPHPFunctions(): Argument #1 ($functions) must be a callable, function "testPhpFunction" not found or invalid function name
Error: No callbacks were registered
