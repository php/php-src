--TEST--
Overriding an EXSLT builtin
--EXTENSIONS--
xsl
--SKIPIF--
<?php
$proc = new xsltprocessor;
if (!$proc->hasExsltSupport()) die('skip EXSLT support not available');
if (LIBXSLT_VERSION < 10130) die('skip too old libxsl');
?>
--FILE--
<?php

function dummy_year($input) {
    var_dump($input[0]->textContent);
    return 'dummy value';
}

function dummy_exit($input) {
    var_dump($input);
    exit("dummy exit");
}

$xsl = <<<XML
<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" 
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:date="http://exslt.org/dates-and-times"
                extension-element-prefixes="date">
<xsl:output method="text"/>
<xsl:template match="date"><xsl:value-of select="date:year(@date)"/></xsl:template>
</xsl:stylesheet>
XML;

$xml = <<<XML
<?xml version="1.0"?>
<page><date date="2007-12-31"/></page>
XML;

$xsldoc = new DOMDocument();
$xsldoc->loadXML($xsl);

$xmldoc = new DOMDocument();
$xmldoc->loadXML($xml);

$proc = new XSLTProcessor();
$proc->importStylesheet($xsldoc);

// Should override the builtin function
$proc->registerPHPFunctionNS('http://exslt.org/dates-and-times', 'year', dummy_year(...));
echo $proc->transformToXML($xmldoc), "\n";

// Should not exit
$proc->registerPHPFunctionNS('http://www.w3.org/1999/XSL/Transform', 'value-of', dummy_exit(...));
echo $proc->transformToXML($xmldoc), "\n";

?>
--EXPECT--
string(10) "2007-12-31"
dummy value
string(10) "2007-12-31"
dummy value
