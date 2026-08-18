--TEST--
Test zend_test_uri_parser() - error - invalid parse_url-based URI
--EXTENSIONS--
zend_test
--FILE--
<?php

try {
    var_dump(zend_test_uri_parser('http://', 'parse_url'));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\InvalidUriException: The specified URI is malformed
