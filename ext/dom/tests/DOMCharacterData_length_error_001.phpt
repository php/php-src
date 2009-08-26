--TEST--
Invalid State Error when getting length on DOMCharacterData out of content.
--CREDIT--
Jason Bouffard <jbouffard1@yahoo.com>
# TestFest Atlanta 2009-05-14
--FILE--
<?php
$character_data = new DOMCharacterData();
print $character_data->length;
?>
--EXPECTF--
Warning: main(): Invalid State Error in %s