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
Fatal error: Class Dummy may not inherit from final class (XMLParser) in %s on line %d
