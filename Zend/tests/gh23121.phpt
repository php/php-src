--TEST--
GH-23121 (Assertion failure in zend_fcc_addref() for an object whose class has no get_closure handler)
--EXTENSIONS--
simplexml
--FILE--
<?php
$sxe = new SimpleXMLElement('<root/>');

var_dump(is_callable($sxe));

try {
    $sxe();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    call_user_func($sxe);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    libxml_set_external_entity_loader($sxe);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

var_dump(libxml_get_external_entity_loader());
?>
--EXPECT--
bool(false)
Error: Object of type SimpleXMLElement is not callable
TypeError: call_user_func(): Argument #1 ($callback) must be a valid callback, no array or string given
TypeError: libxml_set_external_entity_loader(): Argument #1 ($resolver_function) must be a valid callback or null, no array or string given
NULL
