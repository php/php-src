--TEST--
Invalid State Error when getting data on DOMCharacterData out of content.
--CREDITS--
Eric Berg <ehberg@gmail.com>
# TestFest Atlanta 2009-05-14
--EXTENSIONS--
dom
--FILE--
<?php
$character_data = new DOMCharacterData();
try {
    print $character_data->data;
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}
?>
--EXPECT--
DOMException: Invalid State Error
