--TEST--
Bug #78563 Object should not be cloneable
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php

    $parser = xml_parser_create();
    clone $parser;

?>
===DONE===
--EXPECTF--
Fatal error: Uncaught Error: Trying to clone an uncloneable object of class XmlParser in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
