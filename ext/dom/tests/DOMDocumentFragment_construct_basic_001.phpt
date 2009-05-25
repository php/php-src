--TEST--
DOMDocumentFragment::__construct().
--CREDITS--
Eric Lee Stewart <ericleestewart@gmail.com>
# TestFest Atlanta 2009-05-24
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$fragment = new DOMDocumentFragment();
var_dump($fragment);
?>
--EXPECTF--
object(DOMDocumentFragment)#%d (%d) {
}
