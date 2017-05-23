--TEST--
UUID::parse throws UUIDParsingException for non-hexadecimal characters
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

try {
    UUID::parse('01234567-89ab-cdef-0123-456789abcPHP');
}
catch (UUIDParsingException $e) {
    echo $e->getMessage() , "\n";
}

?>
--EXPECT--
Expected hexadecimal digit, but found 'P' (0x50)
