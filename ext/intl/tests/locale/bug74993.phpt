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
--EXPECT--
Function [ <internal:intl> function locale_get_display_language ] {

  - Parameters [2] {
    Parameter #0 [ <required> $locale ]
    Parameter #1 [ <optional> $in_locale ]
  }
}
Function [ <internal:intl> function locale_get_display_name ] {

  - Parameters [2] {
    Parameter #0 [ <required> $locale ]
    Parameter #1 [ <optional> $in_locale ]
  }
}
Function [ <internal:intl> function locale_get_display_region ] {

  - Parameters [2] {
    Parameter #0 [ <required> $locale ]
    Parameter #1 [ <optional> $in_locale ]
  }
}
Function [ <internal:intl> function locale_get_display_script ] {

  - Parameters [2] {
    Parameter #0 [ <required> $locale ]
    Parameter #1 [ <optional> $in_locale ]
  }
}
Function [ <internal:intl> function locale_get_display_variant ] {

  - Parameters [2] {
    Parameter #0 [ <required> $locale ]
    Parameter #1 [ <optional> $in_locale ]
  }
}
Function [ <internal:intl> function locale_filter_matches ] {

  - Parameters [3] {
    Parameter #0 [ <required> $langtag ]
    Parameter #1 [ <required> $locale ]
    Parameter #2 [ <optional> $canonicalize ]
  }
}
Function [ <internal:intl> function locale_lookup ] {

  - Parameters [4] {
    Parameter #0 [ <required> $langtag ]
    Parameter #1 [ <required> $locale ]
    Parameter #2 [ <optional> $canonicalize ]
    Parameter #3 [ <optional> $def ]
  }
}
