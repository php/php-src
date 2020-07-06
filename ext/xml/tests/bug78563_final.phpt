--TEST--
Bug #78563: parsers should not be extendable
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php

class Dummy extends Xmlparser {

}

?>
===DONE===
--EXPECTF--
Fatal error: Class Dummy cannot extend final class XmlParser in %s on line %d
