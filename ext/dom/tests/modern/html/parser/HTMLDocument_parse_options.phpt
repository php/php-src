--TEST--
Dom\HTMLDocument: loading $options check
--EXTENSIONS--
dom
--FILE--
<?php

$tested_options = [
    PHP_INT_MAX,
    LIBXML_BIGLINES,
    LIBXML_PARSEHUGE,
    LIBXML_DTDATTR,
    LIBXML_DTDLOAD,
    LIBXML_DTDVALID,
    LIBXML_HTML_NODEFDTD,
    LIBXML_NOBLANKS,
    LIBXML_NOCDATA,
    LIBXML_NOEMPTYTAG,
    LIBXML_NOENT,
    /*LIBXML_NSCLEAN, this collides with HTML_PARSE_NOIMPLIED */
    LIBXML_XINCLUDE,
    LIBXML_SCHEMA_CREATE,
    LIBXML_NONET,
    LIBXML_NOWARNING,
    LIBXML_PEDANTIC,
];

foreach (["createFromString", "createFromFile"] as $method) {
    echo "--- Method $method ---\n";
    foreach ($tested_options as $options) {
        var_dump($options);
        try {
            Dom\HTMLDocument::{$method}("x", $options);
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }
    }
}

?>
--EXPECTF--
--- Method createFromString ---
int(%d)
ValueError: Dom\HTMLDocument::createFromString(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, Dom\HTML_NO_DEFAULT_NS)
int(4194304)
ValueError: Dom\HTMLDocument::createFromString(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, Dom\HTML_NO_DEFAULT_NS)
int(524288)
ValueError: Dom\HTMLDocument::createFromString(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, Dom\HTML_NO_DEFAULT_NS)
int(8)
ValueError: Dom\HTMLDocument::createFromString(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, Dom\HTML_NO_DEFAULT_NS)
int(4)
ValueError: Dom\HTMLDocument::createFromString(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, Dom\HTML_NO_DEFAULT_NS)
int(16)
ValueError: Dom\HTMLDocument::createFromString(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, Dom\HTML_NO_DEFAULT_NS)
int(4)
ValueError: Dom\HTMLDocument::createFromString(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, Dom\HTML_NO_DEFAULT_NS)
int(256)
ValueError: Dom\HTMLDocument::createFromString(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, Dom\HTML_NO_DEFAULT_NS)
int(16384)
ValueError: Dom\HTMLDocument::createFromString(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, Dom\HTML_NO_DEFAULT_NS)
int(4)
ValueError: Dom\HTMLDocument::createFromString(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, Dom\HTML_NO_DEFAULT_NS)
int(2)
ValueError: Dom\HTMLDocument::createFromString(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, Dom\HTML_NO_DEFAULT_NS)
int(1024)
ValueError: Dom\HTMLDocument::createFromString(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, Dom\HTML_NO_DEFAULT_NS)
int(1)
ValueError: Dom\HTMLDocument::createFromString(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, Dom\HTML_NO_DEFAULT_NS)
int(2048)
ValueError: Dom\HTMLDocument::createFromString(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, Dom\HTML_NO_DEFAULT_NS)
int(64)
ValueError: Dom\HTMLDocument::createFromString(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, Dom\HTML_NO_DEFAULT_NS)
int(128)
ValueError: Dom\HTMLDocument::createFromString(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, Dom\HTML_NO_DEFAULT_NS)
--- Method createFromFile ---
int(%d)
ValueError: Dom\HTMLDocument::createFromFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, Dom\HTML_NO_DEFAULT_NS)
int(4194304)
ValueError: Dom\HTMLDocument::createFromFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, Dom\HTML_NO_DEFAULT_NS)
int(524288)
ValueError: Dom\HTMLDocument::createFromFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, Dom\HTML_NO_DEFAULT_NS)
int(8)
ValueError: Dom\HTMLDocument::createFromFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, Dom\HTML_NO_DEFAULT_NS)
int(4)
ValueError: Dom\HTMLDocument::createFromFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, Dom\HTML_NO_DEFAULT_NS)
int(16)
ValueError: Dom\HTMLDocument::createFromFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, Dom\HTML_NO_DEFAULT_NS)
int(4)
ValueError: Dom\HTMLDocument::createFromFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, Dom\HTML_NO_DEFAULT_NS)
int(256)
ValueError: Dom\HTMLDocument::createFromFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, Dom\HTML_NO_DEFAULT_NS)
int(16384)
ValueError: Dom\HTMLDocument::createFromFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, Dom\HTML_NO_DEFAULT_NS)
int(4)
ValueError: Dom\HTMLDocument::createFromFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, Dom\HTML_NO_DEFAULT_NS)
int(2)
ValueError: Dom\HTMLDocument::createFromFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, Dom\HTML_NO_DEFAULT_NS)
int(1024)
ValueError: Dom\HTMLDocument::createFromFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, Dom\HTML_NO_DEFAULT_NS)
int(1)
ValueError: Dom\HTMLDocument::createFromFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, Dom\HTML_NO_DEFAULT_NS)
int(2048)
ValueError: Dom\HTMLDocument::createFromFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, Dom\HTML_NO_DEFAULT_NS)
int(64)
ValueError: Dom\HTMLDocument::createFromFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, Dom\HTML_NO_DEFAULT_NS)
int(128)
ValueError: Dom\HTMLDocument::createFromFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, Dom\HTML_NO_DEFAULT_NS)
