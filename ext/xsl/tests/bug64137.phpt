--TEST--
Request #64137 (XSLTProcessor::setParameter() should allow both quotes to be used)
--EXTENSIONS--
xsl
--FILE--
<?php

function test(string $input) {
    $xml = new DOMDocument;
    $xml->loadXML('<X/>');

    $xsl = new DOMDocument;
    $xsl->loadXML('<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"><xsl:output method="text"/><xsl:param name="foo"/><xsl:template match="/"><xsl:value-of select="$foo"/></xsl:template></xsl:stylesheet>');

    $xslt = new XSLTProcessor;
    $xslt->importStylesheet($xsl);
    $xslt->setParameter('', 'foo', $input);

    echo $xslt->transformToXml($xml), "\n";
}

test("");
test("a'");
test("a\"");
test("fo'o\"ba'r\"ba'z");
test("\"\"\"fo'o\"ba'r\"ba'z\"\"\"");
test("'''\"\"\"fo'o\"ba'r\"ba'z\"\"\"'''");

?>
--EXPECT--
a'
a"
fo'o"ba'r"ba'z
"""fo'o"ba'r"ba'z"""
'''"""fo'o"ba'r"ba'z"""'''
