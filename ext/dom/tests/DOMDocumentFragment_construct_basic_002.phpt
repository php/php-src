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
--EXPECT--
object(DOMDocumentFragment)#1 (3) {
  ["nodeValue"]=>
  NULL
  ["prefix"]=>
  string(0) ""
  ["textContent"]=>
  string(0) ""
}
