--TEST--
Locale::parseLocale() handles trailing singleton separators
--EXTENSIONS--
intl
--FILE--
<?php
$locales = [
    'en-',
    'en-a-',
    'de-CH-x-',
    'x-',
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
en-a-: array (
  'language' => 'en',
)
de-CH-x-: array (
  'language' => 'de',
  'region' => 'CH',
)
x-: array (
)
