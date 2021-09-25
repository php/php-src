--TEST--
Invalid State Error when getting length on DOMCharacterData out of content.
--CREDITS--
Jason Bouffard <jbouffard1@yahoo.com>
# TestFest Atlanta 2009-05-14
--EXTENSIONS--
dom
--FILE--
<?php
$character_data = new DOMCharacterData();
try {
    print $character_data->length;
} catch (DOMException $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--EXPECT--
Invalid State Error
