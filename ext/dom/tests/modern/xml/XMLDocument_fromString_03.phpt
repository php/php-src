--TEST--
Dom\XMLDocument::createFromString 03
--EXTENSIONS--
dom
--FILE--
<?php

$flags = [
    LIBXML_RECOVER, LIBXML_NOENT, LIBXML_DTDLOAD, LIBXML_DTDATTR, LIBXML_DTDVALID, LIBXML_NOERROR, LIBXML_NOWARNING, LIBXML_NOBLANKS, LIBXML_XINCLUDE, LIBXML_NSCLEAN, LIBXML_NOCDATA, LIBXML_NONET, LIBXML_PEDANTIC, LIBXML_COMPACT, LIBXML_PARSEHUGE, LIBXML_BIGLINES
];

try {
    Dom\XMLDocument::createFromString('<?xml version="1.0"?><container/>', -1);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

foreach ($flags as $flag) {
    var_dump(Dom\XMLDocument::createFromString('<?xml version="1.0"?><container/>', $flag) instanceof Dom\XMLDocument);
}

?>
--EXPECTF--
Dom\XMLDocument::createFromString(): Argument #2 ($options) contains invalid flags (allowed flags: %s)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
