--TEST--
Collection: Dictionary without key type defined
--FILE--
<?php
collection(Dict) Articles<Article>
{
}
?>
--EXPECTF--
Fatal error: Collection dictionaries must have a key type defined in %s on line %d
