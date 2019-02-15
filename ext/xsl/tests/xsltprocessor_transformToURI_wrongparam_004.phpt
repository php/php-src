--TEST--
Check XSLTProcessor::transformToURI() with 3 parameters
--CREDITS--
Rodrigo Prado de Jesus <royopa [at] gmail [dot] com>
--SKIPIF--
<?php extension_loaded('xsl') or die('skip xsl extension is not available'); ?>
--FILE--
<?php
$xml = <<<EOB
<allusers>
 <user>
  <uid>bob</uid>
 </user>
 <user>
  <uid>joe</uid>
 </user>
</allusers>
EOB;
$xsl = <<<EOB
<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
     xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
     xmlns:php="http://php.net/xsl">
<xsl:output method="html" encoding="utf-8" indent="yes"/>
 <xsl:template match="allusers">
  <html><body>
    <h2>Users</h2>
    <table>
    <xsl:for-each select="user">
      <tr><td>
        <xsl:value-of
             select="php:function('ucfirst',string(uid))"/>
      </td></tr>
    </xsl:for-each>
    </table>
  </body></html>
 </xsl:template>
</xsl:stylesheet>
EOB;

$xmldoc = new DOMDocument('1.0', 'utf-8');
$xmldoc->loadXML($xml);

$xsldoc = new DOMDocument('1.0', 'utf-8');
$xsldoc->loadXML($xsl);

$proc = new XSLTProcessor();
$proc->registerPHPFunctions();
$proc->importStyleSheet($xsldoc);

$uri = 'php://output';

echo $proc->transformToURI($xsldoc, $uri, 'stringValue');
?>
--EXPECTF--
Warning: XSLTProcessor::transformToUri() expects exactly 2 parameters, 3 given in %s on line %d
