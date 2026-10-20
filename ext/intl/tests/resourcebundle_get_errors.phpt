--TEST--
Test ResourceBundle::get() missing keys
--EXTENSIONS--
intl
--FILE--
<?php
include "resourcebundle.inc";

$bundle = new ResourceBundle('en_US', BUNDLE);
var_dump($bundle->get('nonexisting'));
var_dump(intl_get_error_message());

var_dump(resourcebundle_get($bundle, 'nonexisting'));
var_dump(intl_get_error_message());

// Make sure accessing existing after non-existing works.
var_dump($bundle->get('teststring'));

?>
--EXPECT--
NULL
string(91) "ResourceBundle::get(): Cannot load resource element 'nonexisting': U_MISSING_RESOURCE_ERROR"
NULL
string(90) "resourcebundle_get(): Cannot load resource element 'nonexisting': U_MISSING_RESOURCE_ERROR"
string(12) "Hello World!"
