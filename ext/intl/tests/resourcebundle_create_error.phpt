--TEST--
ResourceBundle creation errors
--EXTENSIONS--
intl
--FILE--
<?php

try {
    $rb = new ResourceBundle('en_US', 'non-existing');
    var_dump($rb);
    var_dump(intl_get_error_message());
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$rb = resourcebundle_create('en_US', 'non-existing');
var_dump($rb);
var_dump(intl_get_error_message());

require_once "resourcebundle.inc";

try {
    $rb = new ResourceBundle('en_US', BUNDLE, false);
    var_dump($rb);
    var_dump(intl_get_error_message());
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$rb = resourcebundle_create('en_US', BUNDLE, false);
var_dump($rb);
var_dump(intl_get_error_message());

?>
--EXPECT--
IntlException: Constructor failed
NULL
string(81) "resourcebundle_ctor: Cannot load libICU resource bundle: U_MISSING_RESOURCE_ERROR"
IntlException: Constructor failed
NULL
string(81) "resourcebundle_ctor: Cannot load libICU resource bundle: U_MISSING_RESOURCE_ERROR"
