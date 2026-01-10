--TEST--
mb_http_input() errors:
--EXTENSIONS--
mbstring
--FILE--
<?php

try {
    var_dump(mb_http_input('PN'));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(mb_http_input('Q'));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
mb_http_input(): Argument #1 ($type) must be one of "G", "P", "C", "S", "I", or "L"
mb_http_input(): Argument #1 ($type) must be one of "G", "P", "C", "S", "I", or "L"
