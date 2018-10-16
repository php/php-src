--TEST--
Bug #34617 (zend_deactivate: objects_store used after zend_objects_store_destroy is called)
--SKIPIF--
<?php if (!extension_loaded("xml")) print "skip the xml extension not available"; ?>
--FILE--
<?php
class Thing {}
function boom()
{
    $reader = xml_parser_create();
    $thing = new Thing();
    xml_set_object($reader, $thing);
    die("ok\n");
    xml_parser_free($reader);
}
boom();
?>
--EXPECT--
ok
