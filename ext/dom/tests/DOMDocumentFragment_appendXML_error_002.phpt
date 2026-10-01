--TEST--
DOMDocumentFragment::appendXML() with unbound fragment.
--CREDITS--
Eric Lee Stewart <ericleestewart@gmail.com>
# TestFest Atlanta 2009-05-24
--EXTENSIONS--
dom
--FILE--
<?php
$fragment = new DOMDocumentFragment();
try {
    $fragment->appendXML('<bait>crankbait</bait>');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
DOMException: No Modification Allowed Error
