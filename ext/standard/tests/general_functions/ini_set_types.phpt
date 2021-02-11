--TEST--
ini_set() accepts non-strings under strict_types
--FILE--
<?php
declare(strict_types=1);

ini_set('docref_root', null);
var_dump(ini_get('docref_root'));
ini_set('html_errors', true);
var_dump(ini_get('html_errors'));
ini_set('html_errors', false);
var_dump(ini_get('html_errors'));
ini_set('precision', 6);
var_dump(ini_get('precision'));
// Are there any float options?

try {
    ini_set('foo', []);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
string(0) ""
string(1) "1"
string(0) ""
string(1) "6"
ini_set(): Argument #2 ($value) must be of type string|int|float|bool|null
