--TEST--
Fork of bug33853.phpt with exit replaced by throw
--EXTENSIONS--
xsl
--FILE--
<?php

spl_autoload_register(function ($className) {
    var_dump($className);
    throw new Exception("Autoload exception");
});

$xsl = new DomDocument();
$xsl->loadXML('<?xml version="1.0" encoding="iso-8859-1" ?>
<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
xmlns:php="http://php.net/xsl">
<xsl:template match="/">
<xsl:value-of select="php:function(\'TeSt::dateLang\')" />
</xsl:template>
</xsl:stylesheet>');
$inputdom = new DomDocument();
$inputdom->loadXML('<?xml version="1.0" encoding="iso-8859-1" ?>
<today></today>');

$proc = new XsltProcessor();
$proc->registerPhpFunctions();
$xsl = $proc->importStylesheet($xsl);
try {
    $newdom = $proc->transformToDoc($inputdom);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
    echo $e->getPrevious()->getMessage(), "\n";
}
?>
===DONE===
--EXPECT--
string(4) "TeSt"
Invalid callback TeSt::dateLang, class "TeSt" not found
Autoload exception
===DONE===
