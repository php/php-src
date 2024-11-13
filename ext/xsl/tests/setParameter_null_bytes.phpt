--TEST--
setParameter() with null bytes
--EXTENSIONS--
xsl
--FILE--
<?php

$xslt = new XSLTProcessor();

try {
    $xslt->setParameter("", "foo\0", "bar");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $xslt->setParameter("", "foo", "bar\0");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $xslt->setParameter("", [
        "foo\0" => "bar",
    ]);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $xslt->setParameter("", [
        "foo" => "bar\0",
    ]);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
XSLTProcessor::setParameter(): Argument #2 ($name) must not contain any null bytes
XSLTProcessor::setParameter(): Argument #3 ($value) must not contain any null bytes
XSLTProcessor::setParameter(): Argument #3 ($value) must not contain keys with any null bytes
XSLTProcessor::setParameter(): Argument #3 ($value) must not contain values with any null bytes
