--TEST--
Collator::sort() reports comparison conversion errors through intl.error_level
--EXTENSIONS--
intl
--FILE--
<?php
class BadErrorLevelString {
    public function __toString(): string {
        return "\xFF";
    }
}

ini_set("intl.error_level", E_WARNING);
ini_set("intl.use_exceptions", 0);

$coll = new Collator('en_US');
$array = ['b', new BadErrorLevelString(), 'a'];

var_dump($coll->sort($array, Collator::SORT_STRING));
var_dump($coll->getErrorCode() === U_INVALID_CHAR_FOUND);
echo $coll->getErrorMessage(), PHP_EOL;
?>
--EXPECTF--
Deprecated: ini_set(): Using a value different than 0 for intl.error_level is deprecated, as the intl.error_level INI setting is deprecated. Instead the intl.use_exceptions INI setting should be enabled to throw exceptions on errors or intl_get_error_code()/intl_get_error_message() should be used to manually deal with errors in %s on line %d

Warning: Collator::sort(): Error converting string from UTF-8 to UTF-16 in %s on line %d
bool(false)
bool(true)
Collator::sort(): Error converting string from UTF-8 to UTF-16: U_INVALID_CHAR_FOUND
