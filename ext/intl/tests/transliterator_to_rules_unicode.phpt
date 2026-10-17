--TEST--
Transliterator::toRules() escapes Unicode characters and preserves rules
--EXTENSIONS--
intl
--FILE--
<?php
$transliterator = Transliterator::createFromRules('α > a; \U0001F600 > x;');
$rules = $transliterator->toRules();
var_dump(str_contains($rules, 'α'));
var_dump(str_contains($rules, '😀'));

$escaped = transliterator_to_rules($transliterator, escapeUnprintable: true);
var_dump(preg_match('/[^\x00-\x7f]/', $escaped));

foreach ([$rules, $escaped] as $exported) {
    $copy = Transliterator::createFromRules($exported);
    var_dump($copy->transliterate('α😀'));
}

// Preserve embedded NULs in the output; escape them to make the rules parseable.
$transliterator = Transliterator::createFromRules('\u0000 > z;');
var_dump(str_contains($transliterator->toRules(), "\0"));
$escaped = $transliterator->toRules(true);
var_dump(Transliterator::createFromRules($escaped)->transliterate("\0"));
?>
--EXPECT--
bool(true)
bool(true)
int(0)
string(2) "ax"
string(2) "ax"
bool(true)
string(1) "z"
