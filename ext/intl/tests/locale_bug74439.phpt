--TEST--
Bug #74439 Wrong reflection on the Locale methods
--SKIPIF--
<?php if (!extension_loaded('intl')) die('skip intl extension not available'); ?>
--FILE--
<?php

$methods = [
'getDefault',
'acceptFromHttp',
'canonicalize',
'composeLocale',
'getAllVariants',
'getKeywords',
'getPrimaryLanguage',
'getRegion',
'getScript',
'parseLocale',
'setDefault',
'getDisplayLanguage',
'getDisplayName',
'getDisplayRegion',
'getDisplayScript',
'getDisplayVariant',
'filterMatches',
'lookup',
];

foreach ($methods as $method) {
    $rm = new ReflectionMethod(Locale::class, $method);
    printf("%s: %d, %d\n", $method, $rm->getNumberOfParameters(), $rm->getNumberOfRequiredParameters());
}
?>
===DONE===
--EXPECT--
getDefault: 0, 0
acceptFromHttp: 1, 1
canonicalize: 1, 1
composeLocale: 1, 1
getAllVariants: 1, 1
getKeywords: 1, 1
getPrimaryLanguage: 1, 1
getRegion: 1, 1
getScript: 1, 1
parseLocale: 1, 1
setDefault: 1, 1
getDisplayLanguage: 2, 1
getDisplayName: 2, 1
getDisplayRegion: 2, 1
getDisplayScript: 2, 1
getDisplayVariant: 2, 1
filterMatches: 3, 2
lookup: 4, 2
===DONE===
