--TEST--
Collection with unsupported structure type
--FILE--
<?php
collection(Foobar) Articles<int => Article>
{
}
?>
--EXPECTF--
Parse error: syntax error, unexpected identifier "Articles" in %s on line %d
