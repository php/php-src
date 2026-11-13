--TEST--
bug #69168 (DomNode::getNodePath() returns invalid path)
--EXTENSIONS--
xsl
--FILE--
<?php
$xml = <<<EOB
<allusers><user><uid>bob</uid></user><user><uid>joe</uid></user></allusers>
EOB;
$xsl = <<<EOB
<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:php="http://php.net/xsl">
 <xsl:template match="allusers">
    <xsl:for-each select="user">
      <xsl:value-of select="php:function('getPath',uid)"/><br />
    </xsl:for-each>
 </xsl:template>
</xsl:stylesheet>
EOB;

function getPath($input){
  try {
    $input[0]->nodeValue .= 'a';
  } catch (DOMException $e) {
    echo $e->getMessage(), "\n";
  }
  return $input[0]->getNodePath() . ' = ' . $input[0]->nodeValue;
}

$proc = new XSLTProcessor();
$proc->registerPHPFunctions();
$xslDoc = new DOMDocument();
$xslDoc->loadXML($xsl);
@$proc->importStyleSheet($xslDoc);
$xmlDoc = new DOMDocument();
$xmlDoc->loadXML($xml);
echo @$proc->transformToXML($xmlDoc);

// Tests that the callback did not modify the nodes
var_dump($xmlDoc->firstChild->firstChild->firstChild->getNodePath());
var_dump($xmlDoc->firstChild->firstChild->firstChild->nodeValue);
?>
--EXPECT--
Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress
Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress
<?xml version="1.0"?>
/allusers/user[1]/uid = bob<br/>/allusers/user[2]/uid = joe<br/>
string(21) "/allusers/user[1]/uid"
string(3) "bob"
