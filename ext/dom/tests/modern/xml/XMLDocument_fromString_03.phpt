--TEST--
DOM\XMLDocument::createFromString 03
--EXTENSIONS--
dom
--FILE--
<?php

$flags = [
    LIBXML_NOENT, LIBXML_DTDLOAD, LIBXML_DTDATTR, LIBXML_DTDVALID, LIBXML_NOERROR, LIBXML_NOWARNING, LIBXML_NOBLANKS, LIBXML_XINCLUDE, LIBXML_NSCLEAN, LIBXML_NOCDATA, LIBXML_NONET, LIBXML_PEDANTIC, LIBXML_COMPACT, LIBXML_PARSEHUGE, LIBXML_BIGLINES
];

try {
    DOM\XMLDocument::createFromString('<?xml version="1.0"?><container/>', -1);
} catch (ValueError $e) {
    echo $e->getMessage();
}

foreach ($flags as $flag) {
    DOM\XMLDocument::createFromString('<?xml version="1.0"?><container/>', $flag);
}

?>
--EXPECT--
DOM\XMLDocument::createFromString(): Argument #2 ($options) contains invalid flags (allowed flags: LIBXML_NOENT, LIBXML_DTDLOAD, LIBXML_DTDATTR, LIBXML_DTDVALID, LIBXML_NOERROR, LIBXML_NOWARNING, LIBXML_NOBLANKS, LIBXML_XINCLUDE, LIBXML_NSCLEAN, LIBXML_NOCDATA, LIBXML_NONET, LIBXML_PEDANTIC, LIBXML_COMPACT, LIBXML_PARSEHUGE, LIBXML_BIGLINES)
