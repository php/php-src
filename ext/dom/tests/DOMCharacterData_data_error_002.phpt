--TEST--
Invalid State Error when getting data on DOMCharacterData out of content.
--CREDITS--
Eric Berg <ehberg@gmail.com>
# TestFest Atlanta 2009-05-14
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$character_data = new DOMCharacterData();
try {
    print $character_data->data;
} catch (DOMException $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--EXPECT--
Invalid State Error
