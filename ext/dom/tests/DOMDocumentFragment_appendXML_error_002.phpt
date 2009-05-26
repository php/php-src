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
$fragment->appendXML('<bait>crankbait</bait>');
$document->appendChild($fragment);
?>
--EXPECTF--
Fatal error: Uncaught exception 'DOMException' with message 'No Modification Allowed Error' in %s:%d
Stack trace:
#0 %s(%d): DOMDocumentFragment->appendXML('<bait>crankbait...')
#1 {main}
  thrown in %s on line %d