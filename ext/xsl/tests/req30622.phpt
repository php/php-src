--TEST--
Request #30622 (XSLT: xsltProcessor->setParameter() cannot set namespace URI)
--EXTENSIONS--
xsl
--CREDITS--
Based on a test by <ishikawa at arielworks dot com>
--FILE--
<?php

$xmlDom = new DOMDocument();
$xmlDom->loadXML('<root/>');

$xslDom = new DOMDocument();
$xslDom->loadXML(<<<'XML'
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                              xmlns:test="http://www.php.net/test">
    <xsl:param name="foo" select="'EMPTY'"/>
    <xsl:param name="test:foo" select="'EMPTY'"/>

    <xsl:template match="/root">
        <xsl:text>Namespace "NULL": </xsl:text>
        <xsl:value-of select="$foo"/>
        <xsl:text>, Namespace "http://www.php.net/test": </xsl:text>
        <xsl:value-of select="$test:foo"/>
    </xsl:template>
</xsl:stylesheet>
XML);

$proc = new XSLTProcessor();
$proc->importStyleSheet($xslDom);

echo "--- Set both empty and non-empty namespace ---\n";

$proc->setParameter("", "foo", "SET1");
$proc->setParameter("http://www.php.net/test", "foo", "SET2");
var_dump($proc->getParameter("", "foo"));
var_dump($proc->getParameter("http://www.php.net/test", "foo"));

print $proc->transformToXML($xmlDom);

echo "--- Remove empty namespace entry ---\n";

var_dump($proc->removeParameter("", "foo"));
var_dump($proc->getParameter("", "foo"));
var_dump($proc->getParameter("http://www.php.net/test", "foo"));

print $proc->transformToXML($xmlDom);

echo "--- Remove non-empty namespace entry ---\n";

var_dump($proc->removeParameter("http://www.php.net/test", "foo"));
var_dump($proc->getParameter("", "foo"));
var_dump($proc->getParameter("http://www.php.net/test", "foo"));

print $proc->transformToXML($xmlDom);

echo "--- Set via array ---\n";

$proc->setParameter("", ["foo" => "SET1"]);
$proc->setParameter("http://www.php.net/test", ["foo" => "SET2"]);

print $proc->transformToXML($xmlDom);

?>
--EXPECT--
--- Set both empty and non-empty namespace ---
string(4) "SET1"
string(4) "SET2"
<?xml version="1.0"?>
Namespace "NULL": SET1, Namespace "http://www.php.net/test": SET2
--- Remove empty namespace entry ---
bool(true)
bool(false)
string(4) "SET2"
<?xml version="1.0"?>
Namespace "NULL": EMPTY, Namespace "http://www.php.net/test": SET2
--- Remove non-empty namespace entry ---
bool(true)
bool(false)
bool(false)
<?xml version="1.0"?>
Namespace "NULL": EMPTY, Namespace "http://www.php.net/test": EMPTY
--- Set via array ---
<?xml version="1.0"?>
Namespace "NULL": SET1, Namespace "http://www.php.net/test": SET2
