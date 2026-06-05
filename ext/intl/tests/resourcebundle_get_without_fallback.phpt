--TEST--
ResourceBundle::get() rejects fallback results when fallback is disabled with correct error message
--EXTENSIONS--
intl
--FILE--
<?php
function debug($res) {
    if (is_null($res)) {
        $ret = "NULL\n";
    } else {
        $ret = print_r($res, true) . "\n";
    }
    return $ret . sprintf("%5d: %s\n", intl_get_error_code(), intl_get_error_message());
}

$bundle = new ResourceBundle('de_DE', 'ICUDATA');
echo debug($bundle->get('Version', false));

$bundle = resourcebundle_create('de_DE', 'ICUDATA');
echo debug(resourcebundle_get($bundle, 'Version', false));
?>
--EXPECTF--
NULL
 %i: ResourceBundle::get(): Cannot load element 'Version' without fallback to %s: U_USING_%s_WARNING
NULL
 %i: resourcebundle_get(): Cannot load element 'Version' without fallback to %s: U_USING_%s_WARNING
