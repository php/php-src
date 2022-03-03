--TEST--
DOMDocumentFragment::__construct() called twice.
--CREDITS--
Eric Lee Stewart <ericleestewart@gmail.com>
# TestFest Atlanta 2009-05-24
--EXTENSIONS--
dom
--FILE--
<?php
$fragment = new DOMDocumentFragment();
$fragment->__construct();
var_dump($fragment);
?>
--EXPECT--
object(DOMDocumentFragment)#1 (0) {
}
