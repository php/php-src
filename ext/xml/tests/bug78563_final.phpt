--TEST--
Bug #78563: parsers should not be extendable
--EXTENSIONS--
xml
--FILE--
<?php

class Dummy extends Xmlparser {

}

?>
===DONE===
--EXPECTF--
Fatal error: Class Dummy cannot extend final class XMLParser in %s on line %d
