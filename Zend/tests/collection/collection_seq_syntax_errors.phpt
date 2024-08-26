--TEST--
Collection: Sequence with key type defined
--FILE--
<?php
collection(Seq) Articles<int => Article>
{
}
?>
--EXPECTF--
Fatal error: Collection sequences may not have a key type defined in %s on line %d
