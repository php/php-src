--TEST--
DOMDocumentFragment::__construct() called twice.
--CREDITS--
Eric Lee Stewart <ericleestewart@gmail.com>
# TestFest Atlanta 2009-05-24
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$fragment = new DOMDocumentFragment();
$fragment->__construct();
var_dump($fragment);
?>
--EXPECTF--
object(DOMDocumentFragment)#%d (%d) {
}
