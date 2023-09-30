--TEST--
setParameter exceptions test
--EXTENSIONS--
xsl
--FILE--
<?php

function test(array $options) {
    $xml = new DOMDocument;
    $xml->loadXML('<X/>');

    $xsl = new DOMDocument;
    $xsl->loadXML('<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"><xsl:output method="text"/><xsl:param name="foo"/><xsl:template match="/"><xsl:value-of select="$foo"/></xsl:template></xsl:stylesheet>');

    $xslt = new XSLTProcessor;
    $xslt->importStylesheet($xsl);
    $xslt->setParameter('', $options);

    echo $xslt->transformToXml($xml), "\n";
}

echo "--- Invalid key ---\n";

try {
    test([
        12345 => "foo"
    ]);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "--- Valid key and value, but special cases ---\n";

test([
    "foo" => null,
]);

test([
    "foo" => true,
]);

echo "--- Exception from Stringable should abort execution ---\n";

class MyStringable {
    public function __toString(): string {
        throw new Exception("exception!");
    }
}

try {
    test([
        "foo" => new MyStringable,
    ]);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

echo "--- Exception from warning should abort execution ---\n";

set_error_handler(function($errno, $errstr) {
    throw new Exception($errstr);
}, E_WARNING);

try {
    test([
        "foo" => [],
        "foo2" => [],
    ]);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

set_error_handler(null, E_WARNING);

echo "--- Warning may continue execution ---\n";

try {
    test([
        "foo" => [],
        "foo2" => [],
    ]);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
--- Invalid key ---
XSLTProcessor::setParameter(): Argument #2 ($name) must contain only string keys
--- Valid key and value, but special cases ---

1
--- Exception from Stringable should abort execution ---
exception!
--- Exception from warning should abort execution ---
Array to string conversion
--- Warning may continue execution ---

Warning: Array to string conversion in %s on line %d

Warning: Array to string conversion in %s on line %d
Array
