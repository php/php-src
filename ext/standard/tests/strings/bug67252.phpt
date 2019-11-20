--TEST--
Bug #67252 (convert_uudecode out-of-bounds read)
--FILE--
<?php
try {
    convert_uudecode("M86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A" . "\n" . "a.");
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--EXPECT--
The given parameter is not a valid uuencoded string
