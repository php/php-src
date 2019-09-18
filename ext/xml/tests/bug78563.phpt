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
--EXPECT--
===DONE===
