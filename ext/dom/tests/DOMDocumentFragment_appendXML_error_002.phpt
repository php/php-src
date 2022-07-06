--TEST--
DOMDocumentFragment::appendXML() with unbound fragment.
--CREDITS--
Eric Lee Stewart <ericleestewart@gmail.com>
# TestFest Atlanta 2009-05-24
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$fragment = new DOMDocumentFragment();
try {
    $fragment->appendXML('<bait>crankbait</bait>');
} catch (DOMException $e) {
    echo $e->getMessage();
}
?>
--EXPECT--
No Modification Allowed Error
