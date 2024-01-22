--TEST--
DOM\HTMLDocument: loading $options check
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
            DOM\HTMLDocument::{$method}("x", $options);
        } catch (ValueError $e) {
            echo $e->getMessage(), "\n";
        }
    }
}

?>
--EXPECTF--
--- Method createFromString ---
int(%d)
DOM\HTMLDocument::createFromString(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(4194304)
DOM\HTMLDocument::createFromString(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(524288)
DOM\HTMLDocument::createFromString(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(8)
DOM\HTMLDocument::createFromString(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(4)
DOM\HTMLDocument::createFromString(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(16)
DOM\HTMLDocument::createFromString(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(4)
DOM\HTMLDocument::createFromString(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(256)
DOM\HTMLDocument::createFromString(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(16384)
DOM\HTMLDocument::createFromString(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(4)
DOM\HTMLDocument::createFromString(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(2)
DOM\HTMLDocument::createFromString(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(1024)
DOM\HTMLDocument::createFromString(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(1)
DOM\HTMLDocument::createFromString(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(2048)
DOM\HTMLDocument::createFromString(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(64)
DOM\HTMLDocument::createFromString(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(128)
DOM\HTMLDocument::createFromString(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
--- Method createFromFile ---
int(%d)
DOM\HTMLDocument::createFromFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(4194304)
DOM\HTMLDocument::createFromFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(524288)
DOM\HTMLDocument::createFromFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(8)
DOM\HTMLDocument::createFromFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(4)
DOM\HTMLDocument::createFromFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(16)
DOM\HTMLDocument::createFromFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(4)
DOM\HTMLDocument::createFromFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(256)
DOM\HTMLDocument::createFromFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(16384)
DOM\HTMLDocument::createFromFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(4)
DOM\HTMLDocument::createFromFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(2)
DOM\HTMLDocument::createFromFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(1024)
DOM\HTMLDocument::createFromFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(1)
DOM\HTMLDocument::createFromFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(2048)
DOM\HTMLDocument::createFromFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(64)
DOM\HTMLDocument::createFromFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(128)
DOM\HTMLDocument::createFromFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
