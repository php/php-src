--TEST--
Bug #61087 (Memory leak in parse_ini_file when specifying invalid scanner mode)
--FILE--
<?php
// the used file is actually irrelevant, so just use this file
// even though it's not an .ini
parse_ini_file(__FILE__, false, 100);
?>
--EXPECTF--
Warning: Invalid scanner mode in %s on line %d
