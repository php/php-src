--TEST--
Locale::parseLocale() does not read past a trailing '-' or '_'
--EXTENSIONS--
intl
--FILE--
<?php
/* Enough lengths that the byte past the end clears the allocation. */
foreach (['-', '_'] as $sep) {
    for ($len = 1; $len <= 64; $len++) {
        Locale::parseLocale(str_repeat('a', $len) . $sep);
    }
}

$locales = [
    'en-',
    'foo-',
    'en_US-',
    'en_',
    'de-CH-x-',
];

foreach ($locales as $locale) {
    echo $locale, ': ';
    var_export(Locale::parseLocale($locale));
    echo "\n";
}
?>
--EXPECT--
en-: array (
  'language' => 'en',
)
foo-: array (
  'language' => 'foo',
)
en_US-: array (
  'language' => 'en',
  'region' => 'US',
)
en_: array (
  'language' => 'en',
)
de-CH-x-: array (
  'language' => 'de',
  'region' => 'CH',
)
