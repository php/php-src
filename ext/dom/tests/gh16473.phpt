--TEST--
GH-16473 (dom_import_simplexml stub is wrong)
--EXTENSIONS--
dom
simplexml
--FILE--
<?php
$root = simplexml_load_string('<root xmlns:x="urn:x" x:attr="foo"/>');
$attr = $root->attributes('urn:x');
var_dump(dom_import_simplexml($attr)->textContent);
?>
--EXPECT--
string(3) "foo"
