--TEST--
ResourceBundle::get() rejects fallback results when fallback is disabled with correct error message
--EXTENSIONS--
intl
--FILE--
<?php
require_once "resourcebundle.inc";

$fixture = __DIR__ . '/resourcebundle_get_without_fallback_tmp';
if (!is_dir($fixture)) {
    mkdir($fixture);
}
copy(BUNDLE . '/root.res', $fixture . '/root.res');

$resIndex = file_get_contents(BUNDLE . '/res_index.res');
$resIndex = str_replace("\0es\0root", "\0en\0root", $resIndex, $replacementCount);
if ($replacementCount !== 1) {
    die("Failed to prepare resource bundle index\n");
}
file_put_contents($fixture . '/res_index.res', $resIndex);

// Keep the binary layout intact while making this key fall back to root.res.
$enBundle = file_get_contents(BUNDLE . '/es.res');
$enBundle = str_replace('teststring', 'teststrinh', $enBundle, $replacementCount);
if ($replacementCount !== 1) {
    die("Failed to prepare resource bundle fixture\n");
}
file_put_contents($fixture . '/en.res', $enBundle);

$bundle = new ResourceBundle('en', $fixture);
echo debug($bundle->get('teststring', false));

$bundle = resourcebundle_create('en', $fixture);
echo debug(resourcebundle_get($bundle, 'teststring', false));
?>
--EXPECTF--
NULL
 %i: ResourceBundle::get(): Cannot load element 'teststring' without fallback to %s: U_USING_%s_WARNING
NULL
 %i: resourcebundle_get(): Cannot load element 'teststring' without fallback to %s: U_USING_%s_WARNING
--CLEAN--
<?php
$fixture = __DIR__ . '/resourcebundle_get_without_fallback_tmp';
@unlink($fixture . '/en.res');
@unlink($fixture . '/root.res');
@unlink($fixture . '/res_index.res');
@rmdir($fixture);
?>
