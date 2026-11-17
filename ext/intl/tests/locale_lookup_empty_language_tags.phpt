--TEST--
Locale::lookup() honors the default locale for an empty language tag list
--EXTENSIONS--
intl
--FILE--
<?php
foreach (['locale_lookup', [Locale::class, 'lookup']] as $lookup) {
    foreach ([false, true] as $canonicalize) {
        var_dump($lookup([], 'fr_FR', $canonicalize, 'en-US'));
        var_dump($lookup([], '', $canonicalize, 'en-US'));
        var_dump($lookup([], 'fr_FR', $canonicalize, ''));
        var_dump($lookup([], 'fr_FR', $canonicalize, null));
        var_dump($lookup([], 'fr_FR', $canonicalize));
        var_dump(intl_get_error_code());
    }
}
?>
--EXPECT--
string(5) "en-US"
string(5) "en-US"
string(0) ""
string(0) ""
string(0) ""
int(0)
string(5) "en-US"
string(5) "en-US"
string(0) ""
string(0) ""
string(0) ""
int(0)
string(5) "en-US"
string(5) "en-US"
string(0) ""
string(0) ""
string(0) ""
int(0)
string(5) "en-US"
string(5) "en-US"
string(0) ""
string(0) ""
string(0) ""
int(0)
