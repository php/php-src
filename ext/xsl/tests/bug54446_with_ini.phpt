--TEST--
Bug #54446 (Arbitrary file creation via libxslt 'output' extension with php.ini setting)
--SKIPIF--
<?php
if (!extension_loaded('xsl')) die("skip Extension XSL is required\n");
?>
--FILE--
<?php
include("prepare.inc");

$outputfile = __DIR__."/bug54446test_with_ini.txt";
if (file_exists($outputfile)) {
    unlink($outputfile);
}

$sXsl = <<<EOT
<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:sax="http://icl.com/saxon"
    extension-element-prefixes="sax">

    <xsl:template match="/">
        <sax:output href="$outputfile" method="text">
            <xsl:value-of select="'0wn3d via PHP and libxslt ...'"/>
        </sax:output>
    </xsl:template>

</xsl:stylesheet>
EOT;

$xsl->loadXML( $sXsl );

# START XSLT
$proc->importStylesheet( $xsl );

# TRASNFORM & PRINT
print $proc->transformToXML( $dom );


if (file_exists($outputfile)) {
    print "$outputfile exists, but shouldn't!\n";
} else {
    print "OK, no file created\n";
}

#SET NO SECURITY PREFS
$proc->setSecurityPrefs(XSL_SECPREF_NONE);

# TRANSFORM & PRINT
print $proc->transformToXML( $dom );


if (file_exists($outputfile)) {
    print "OK, file exists\n";
} else {
    print "$outputfile doesn't exist, but should!\n";
}

unlink($outputfile);

#SET SECURITY PREFS AGAIN
$proc->setSecurityPrefs(XSL_SECPREF_WRITE_FILE | XSL_SECPREF_WRITE_NETWORK | XSL_SECPREF_CREATE_DIRECTORY);

# TRANSFORM & PRINT
print $proc->transformToXML( $dom );

if (file_exists($outputfile)) {
    print "$outputfile exists, but shouldn't!\n";
} else {
    print "OK, no file created\n";
}

?>
--EXPECTF--
Warning: XSLTProcessor::transformToXml(): runtime error: file %s line %s element output in %s on line %d

Warning: XSLTProcessor::transformToXml(): File write for %s/bug54446test_with_ini.txt refused in %s on line %d

Warning: XSLTProcessor::transformToXml(): runtime error: file %s line %d element output in %s on line %d

Warning: XSLTProcessor::transformToXml(): xsltDocumentElem: write rights for %s/bug54446test_with_ini.txt denied in %s on line %d
OK, no file created
OK, file exists

Warning: XSLTProcessor::transformToXml(): runtime error: file %s line %s element output in %s on line %d

Warning: XSLTProcessor::transformToXml(): File write for %s/bug54446test_with_ini.txt refused in %s on line %d

Warning: XSLTProcessor::transformToXml(): runtime error: file %s line %d element output in %s on line %d

Warning: XSLTProcessor::transformToXml(): xsltDocumentElem: write rights for %s/bug54446test_with_ini.txt denied in %s on line %d
OK, no file created
--CREDITS--
Christian Stocker, chregu@php.net
