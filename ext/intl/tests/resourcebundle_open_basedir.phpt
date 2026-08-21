--TEST--
ResourceBundle bundle paths respect open_basedir
--EXTENSIONS--
intl
--INI--
open_basedir=.
display_errors=1
log_errors=0
--FILE--
<?php
chdir(__DIR__);
require_once "resourcebundle.inc";

var_dump(ResourceBundle::create('root', BUNDLE) instanceof ResourceBundle);
var_dump(ResourceBundle::getLocales(BUNDLE));

$outside = dirname(__DIR__);

try {
    new ResourceBundle('root', $outside);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

var_dump(ResourceBundle::create('root', $outside));
var_dump(ResourceBundle::getLocales($outside));
?>
--EXPECTF--
bool(true)
array(2) {
  [0]=>
  string(2) "es"
  [1]=>
  string(4) "root"
}

Warning: ResourceBundle::__construct(): open_basedir restriction in effect. File(%s) is not within the allowed path(s): (.) in %s on line %d
IntlException: ResourceBundle::__construct(): open_basedir restriction in effect

Warning: ResourceBundle::create(): open_basedir restriction in effect. File(%s) is not within the allowed path(s): (.) in %s on line %d
NULL

Warning: ResourceBundle::getLocales(): open_basedir restriction in effect. File(%s) is not within the allowed path(s): (.) in %s on line %d
bool(false)
