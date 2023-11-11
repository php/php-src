--TEST--
php:function() edge cases
--EXTENSIONS--
xsl
--FILE--
<?php

function test($input) {
    $xsl = new DomDocument();
    $xsl->loadXML('<?xml version="1.0" encoding="iso-8859-1" ?>
    <xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:php="http://php.net/xsl">
    <xsl:template match="/">
    <xsl:value-of select="php:function(' . $input . ')" />
    </xsl:template>
    </xsl:stylesheet>');

    $inputdom = new DomDocument();
    $inputdom->loadXML('<?xml version="1.0" encoding="iso-8859-1" ?>
    <today></today>');

    $proc = new XsltProcessor();
    $proc->registerPhpFunctions();
    $xsl = $proc->importStylesheet($xsl);
    try {
        $proc->transformToDoc($inputdom);
    } catch (Exception $e) {
        echo $e->getMessage(), "\n";
    }
}

try {
    test("");
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

test("3");

?>
--EXPECTF--
Function name must be passed as the first argument

Warning: XSLTProcessor::transformToDoc(): Handler name must be a string in %s on line %d
