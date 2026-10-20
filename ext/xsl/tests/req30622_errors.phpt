--TEST--
Request #30622 (XSLT: xsltProcessor->setParameter() cannot set namespace URI) - error cases
--EXTENSIONS--
xsl
--CREDITS--
Based on a test by <ishikawa at arielworks dot com>
--FILE--
<?php

$proc = new XSLTProcessor();

try {
    $proc->setParameter("urn:x", "{urn:a}x", "");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $proc->setParameter("urn:x", "a:b", "");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $proc->getParameter("urn:x", "{urn:a}x");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $proc->getParameter("urn:x", "a:b");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $proc->removeParameter("urn:x", "{urn:a}x");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $proc->removeParameter("urn:x", "a:b");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

// Edge cases, should work
$proc->setParameter("urn:x", ":b", "");
$proc->setParameter("urn:x", ":", "");

?>
--EXPECT--
XSLTProcessor::setParameter(): Argument #2 ($name) must not use clark notation when argument #1 ($namespace) is not empty
XSLTProcessor::setParameter(): Argument #2 ($name) must not be a QName when argument #1 ($namespace) is not empty
XSLTProcessor::getParameter(): Argument #2 ($name) must not use clark notation when argument #1 ($namespace) is not empty
XSLTProcessor::getParameter(): Argument #2 ($name) must not be a QName when argument #1 ($namespace) is not empty
XSLTProcessor::removeParameter(): Argument #2 ($name) must not use clark notation when argument #1 ($namespace) is not empty
XSLTProcessor::removeParameter(): Argument #2 ($name) must not be a QName when argument #1 ($namespace) is not empty
