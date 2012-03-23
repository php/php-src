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
var_dump(get_class($fragment));
?>
--EXPECT--
string(19) "DOMDocumentFragment"
