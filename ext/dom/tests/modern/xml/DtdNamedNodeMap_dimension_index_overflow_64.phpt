--TEST--
Dom\DtdNamedNodeMap dimension access handles invalid integer indexes
--EXTENSIONS--
dom
--SKIPIF--
<?php
if (PHP_INT_SIZE == 4) {
    die("skip this test is for 64bit platform only");
}
?>
--FILE--
<?php
$doc = Dom\XMLDocument::createFromString(<<<XML
<!DOCTYPE root [
<!ENTITY e "entity">
<!NOTATION n SYSTEM "notation">
]>
<root/>
XML);

$overflow = 4294967296;

function dump_access(Closure $callback): void {
    try {
        var_dump($callback()?->nodeName);
    } catch (ValueError $e) {
        echo $e->getMessage(), PHP_EOL;
    }
}

dump_access(fn() => $doc->doctype->entities[-1]);
dump_access(fn() => $doc->doctype->entities[$overflow]);

dump_access(fn() => $doc->doctype->notations[-1]);
dump_access(fn() => $doc->doctype->notations[$overflow]);
?>
--EXPECT--
NULL
must be between 0 and 2147483647
NULL
must be between 0 and 2147483647
