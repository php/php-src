--TEST--
ini_get_all() accepts case-insensitive extension names
--FILE--
<?php
foreach (['PCRE', 'Pcre', 'pCrE'] as $extension) {
    var_dump(ini_get_all($extension) === ini_get_all('pcre'));
    var_dump(ini_get_all($extension, true) === ini_get_all('pcre', true));
    var_dump(ini_get_all($extension, false) === ini_get_all('pcre', false));
}

var_dump(ini_get_all('REFLECTION'));
var_dump(ini_get_all('Reflection', false));

var_dump(ini_get_all('NoSuchExtension'));
var_dump(ini_get_all('NoSuchExtension', false));
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
array(0) {
}
array(0) {
}

Warning: ini_get_all(): Extension "NoSuchExtension" cannot be found in %s on line %d
bool(false)

Warning: ini_get_all(): Extension "NoSuchExtension" cannot be found in %s on line %d
bool(false)
