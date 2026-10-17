--TEST--
Transliterator::toRules() and transliterator_to_rules()
--EXTENSIONS--
intl
--FILE--
<?php
$transliterator = Transliterator::createFromRules('a > b;');
var_dump($transliterator->toRules());
var_dump(transliterator_to_rules($transliterator));
var_dump($transliterator->toRules(escapeUnprintable: true));

foreach ([
    $transliterator,
    Transliterator::create('Any-Lower; Latin-ASCII'),
    Transliterator::createFromRules('a <> b;', Transliterator::REVERSE),
    Transliterator::createFromRules(''),
    Transliterator::createFromRules('a > ' . str_repeat('b', 4096) . ';'),
] as $transliterator) {
    foreach ([false, true] as $escapeUnprintable) {
        $rules = $transliterator->toRules($escapeUnprintable);
        $copy = Transliterator::createFromRules($rules);
        var_dump($copy->transliterate('ab ÄÉ') === $transliterator->transliterate('ab ÄÉ'));
    }
}
?>
--EXPECT--
string(6) "a > b;"
string(6) "a > b;"
string(6) "a > b;"
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
