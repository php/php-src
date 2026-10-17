--TEST--
Transliterator::toRules() validates objects and resets previous errors
--EXTENSIONS--
intl
--INI--
intl.error_level=0
--FILE--
<?php
$transliterator = Transliterator::createFromRules('a > b;');
foreach ([
    fn() => $transliterator->toRules(),
    fn() => transliterator_to_rules($transliterator),
] as $export) {
    var_dump($transliterator->transliterate("\xff"));
    var_dump(intl_is_failure($transliterator->getErrorCode()));
    var_dump($export());
    var_dump($transliterator->getErrorCode(), intl_get_error_code());
}

class UnconstructedTransliterator extends Transliterator {}
$unconstructed = (new ReflectionClass(UnconstructedTransliterator::class))->newInstanceWithoutConstructor();
foreach ([
    fn() => $unconstructed->toRules(),
    fn() => transliterator_to_rules($unconstructed),
    fn() => $transliterator->toRules([]),
] as $export) {
    try {
        $export();
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}
?>
--EXPECT--
bool(false)
bool(true)
string(6) "a > b;"
int(0)
int(0)
bool(false)
bool(true)
string(6) "a > b;"
int(0)
int(0)
Found unconstructed transliterator
Found unconstructed transliterator
Transliterator::toRules(): Argument #1 ($escapeUnprintable) must be of type bool, array given
