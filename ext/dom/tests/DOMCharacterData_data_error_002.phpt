--TEST--
Invalid State Error when getting data on DOMCharacterData out of content.
--CREDIT--
Eric Berg <ehberg@gmail.com>
# TestFest Atlanta 2009-05-14
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$character_data = new DOMCharacterData();
print $character_data->data;
?>
--EXPECTF--
Warning: main(): Invalid State Error in %s on line %d