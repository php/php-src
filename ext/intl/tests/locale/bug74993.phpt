--TEST--
Bug #74993 Wrong reflection on some locale_* functions
--SKIPIF--
<?php
extension_loaded('intl') || print 'skip';
--FILE--
<?php
$funcs = [
    'locale_get_display_language',
    'locale_get_display_name',
    'locale_get_display_region',
    'locale_get_display_script',
    'locale_get_display_variant',
    'locale_filter_matches',
    'locale_lookup',
];

foreach ($funcs as $func) {
    echo (new ReflectionFunction($func));
}
?>
--EXPECT--
Function [ <internal:intl> function locale_get_display_language ] {

  - Parameters [2] {
    Parameter #0 [ <required> string $locale ]
    Parameter #1 [ <optional> ?string $displayLocale = null ]
  }
  - Return [ string|false ]
}
Function [ <internal:intl> function locale_get_display_name ] {

  - Parameters [2] {
    Parameter #0 [ <required> string $locale ]
    Parameter #1 [ <optional> ?string $displayLocale = null ]
  }
  - Return [ string|false ]
}
Function [ <internal:intl> function locale_get_display_region ] {

  - Parameters [2] {
    Parameter #0 [ <required> string $locale ]
    Parameter #1 [ <optional> ?string $displayLocale = null ]
  }
  - Return [ string|false ]
}
Function [ <internal:intl> function locale_get_display_script ] {

  - Parameters [2] {
    Parameter #0 [ <required> string $locale ]
    Parameter #1 [ <optional> ?string $displayLocale = null ]
  }
  - Return [ string|false ]
}
Function [ <internal:intl> function locale_get_display_variant ] {

  - Parameters [2] {
    Parameter #0 [ <required> string $locale ]
    Parameter #1 [ <optional> ?string $displayLocale = null ]
  }
  - Return [ string|false ]
}
Function [ <internal:intl> function locale_filter_matches ] {

  - Parameters [3] {
    Parameter #0 [ <required> string $languageTag ]
    Parameter #1 [ <required> string $locale ]
    Parameter #2 [ <optional> bool $canonicalize = false ]
  }
  - Return [ ?bool ]
}
Function [ <internal:intl> function locale_lookup ] {

  - Parameters [4] {
    Parameter #0 [ <required> array $languageTag ]
    Parameter #1 [ <required> string $locale ]
    Parameter #2 [ <optional> bool $canonicalize = false ]
    Parameter #3 [ <optional> ?string $defaultLocale = null ]
  }
  - Return [ ?string ]
}
