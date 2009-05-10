--TEST--
Bug #48221 (memory leak when passing invalid xslt parameter)
--SKIPIF--
<?php
if (!extension_loaded('xsl')) die("skip Extension XSL is required\n");
?>
--FILE--
<?php

$xsl = new DOMDocument;
$xsl->loadXML('<html xsl:version="1.0"
      xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
      lang="en">
    <head>
        <title>Sales Results By Division</title>
    </head>
    <body>
        <table border="1">
            <tr>
                <th>Division</th>
                <th>Revenue</th>
                <th>Growth</th>
                <th>Bonus</th>
            </tr>
            <xsl:for-each select="sales/division">
                <!-- order the result by revenue -->
                <xsl:sort select="revenue"
                          data-type="number"
                          order="descending"/>
                <tr>
                    <td>
                        <em><xsl:value-of select="@id"/></em>
                    </td>
                    <td>
                        <xsl:value-of select="revenue"/>
                    </td>
                    <td>
                        <!-- highlight negative growth in red -->
                        <xsl:if test="growth &lt; 0">
                             <xsl:attribute name="style">
                                 <xsl:text>color:red</xsl:text>
                             </xsl:attribute>
                        </xsl:if>
                        <xsl:value-of select="growth"/>
                    </td>
                    <td>
                        <xsl:value-of select="bonus"/>
                    </td>
                </tr>
            </xsl:for-each>
        </table>
    </body>
</html>');

$dom = new DOMDocument;
$dom->loadXMl('<sales>

        <division id="North">
                <revenue>10</revenue>
                <growth>9</growth>
                <bonus>7</bonus>
        </division>

        <division id="South">
                <revenue>4</revenue>
                <growth>3</growth>
                <bonus>4</bonus>
        </division>

        <division id="West">
                <revenue>6</revenue>
                <growth>-1.5</growth>
                <bonus>2</bonus>
        </division>

</sales>');

$proc = new xsltprocessor;
$proc->importStylesheet($xsl);
$proc->setParameter('', '', '"\'');
$proc->transformToXml($dom);

?>
--EXPECTF--
Warning: XSLTProcessor::transformToXml(): Cannot create XPath expression (string contains both quote and double-quotes) in %s on line %d
