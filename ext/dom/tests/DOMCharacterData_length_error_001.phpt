--TEST--
Invalid State Error when getting length on DOMCharacterData out of content.
--CREDITS--
Jason Bouffard <jbouffard1@yahoo.com>
# TestFest Atlanta 2009-05-14
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$character_data = new DOMCharacterData();
print $character_data->length;
?>
--EXPECTF--
Warning: main(): Invalid State Error in %s
