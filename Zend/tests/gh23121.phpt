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
} catch (Error $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    call_user_func($sxe);
} catch (TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    libxml_set_external_entity_loader($sxe);
} catch (TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
}

var_dump(libxml_get_external_entity_loader());
?>
--EXPECT--
bool(false)
Object of type SimpleXMLElement is not callable
call_user_func(): Argument #1 ($callback) must be a valid callback, no array or string given
libxml_set_external_entity_loader(): Argument #1 ($resolver_function) must be a valid callback or null, no array or string given
NULL
