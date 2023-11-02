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
    $proc->importStylesheet($xsl);
    $proc->transformToDoc($inputdom);
}

try {
    test("");
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

try {
    test("3");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Function name must be passed as the first argument
Handler name must be a string
