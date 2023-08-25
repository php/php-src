--TEST--
HTML5Document: loading $options check
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOM\HTML5Document();

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

foreach (["loadHTML", "loadHTMLFile"] as $method) {
    echo "--- Method $method ---\n";
    foreach ($tested_options as $options) {
        var_dump($options);
        try {
            $dom->{$method}("x", $options);
        } catch (ValueError $e) {
            echo $e->getMessage(), "\n";
        }
    }
}

?>
--EXPECTF--
--- Method loadHTML ---
int(%d)
DOM\HTML5Document::loadHTML(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(4194304)
DOM\HTML5Document::loadHTML(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(524288)
DOM\HTML5Document::loadHTML(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(8)
DOM\HTML5Document::loadHTML(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(4)
DOM\HTML5Document::loadHTML(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(16)
DOM\HTML5Document::loadHTML(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(4)
DOM\HTML5Document::loadHTML(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(256)
DOM\HTML5Document::loadHTML(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(16384)
DOM\HTML5Document::loadHTML(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(4)
DOM\HTML5Document::loadHTML(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(2)
DOM\HTML5Document::loadHTML(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(1024)
DOM\HTML5Document::loadHTML(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(1)
DOM\HTML5Document::loadHTML(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(2048)
DOM\HTML5Document::loadHTML(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(64)
DOM\HTML5Document::loadHTML(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(128)
DOM\HTML5Document::loadHTML(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
--- Method loadHTMLFile ---
int(%d)
DOM\HTML5Document::loadHTMLFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(4194304)
DOM\HTML5Document::loadHTMLFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(524288)
DOM\HTML5Document::loadHTMLFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(8)
DOM\HTML5Document::loadHTMLFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(4)
DOM\HTML5Document::loadHTMLFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(16)
DOM\HTML5Document::loadHTMLFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(4)
DOM\HTML5Document::loadHTMLFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(256)
DOM\HTML5Document::loadHTMLFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(16384)
DOM\HTML5Document::loadHTMLFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(4)
DOM\HTML5Document::loadHTMLFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(2)
DOM\HTML5Document::loadHTMLFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(1024)
DOM\HTML5Document::loadHTMLFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(1)
DOM\HTML5Document::loadHTMLFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(2048)
DOM\HTML5Document::loadHTMLFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(64)
DOM\HTML5Document::loadHTMLFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
int(128)
DOM\HTML5Document::loadHTMLFile(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOERROR, LIBXML_COMPACT, LIBXML_HTML_NOIMPLIED, DOM\NO_DEFAULT_NS)
