--TEST--
Bug #71540 (NULL pointer dereference in xsl_ext_function_php())
--SKIPIF--
<?php
if (!extension_loaded('xsl')) die("skip Extension XSL is required\n");
if (LIBXML_VERSION >= 20903) die('skip this test is for PHP linked with libxml2 < 2.9.3 only')
?>
--FILE--
<?php
$xml = <<<EOB
<allusers>
 <user>
  <uid>bob</uid>
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
             select="php:function('test',uid,test(test))"/>
      </td></tr>
    </xsl:for-each>
    </table>
  </body></html>
 </xsl:template>
</xsl:stylesheet>
EOB;

$xmldoc = new DOMDocument();
$xmldoc->loadXML($xml);
$xsldoc = new DOMDocument();
$xsldoc->loadXML($xsl);

$proc = new XSLTProcessor();
$proc->registerPHPFunctions();
$proc->importStyleSheet($xsldoc);
echo $proc->transformToXML($xmldoc);
?>
DONE
--EXPECTF--
Warning: XSLTProcessor::transformToXml(): xmlXPathCompOpEval: function test not found in %sbug71540.php on line %d

Warning: XSLTProcessor::transformToXml(): Unregistered function in %sbug71540.php on line %d

Warning: XSLTProcessor::transformToXml(): Stack usage errror in %sbug71540.php on line %d

Warning: XSLTProcessor::transformToXml(): Stack usage errror in %sbug71540.php on line %d

Warning: XSLTProcessor::transformToXml(): Handler name must be a string in %sbug71540.php on line %d

Warning: XSLTProcessor::transformToXml(): xmlXPathCompiledEval: 2 objects left on the stack. in %sbug71540.php on line %d

Warning: XSLTProcessor::transformToXml(): runtime error: file %s line 13 element value-of in %sbug71540.php on line %d

Warning: XSLTProcessor::transformToXml(): XPath evaluation returned no result. in %sbug71540.php on line %d
<html xmlns:php="http://php.net/xsl"><body>
<h2>Users</h2>
<table><tr><td></td></tr></table>
</body></html>
DONE
