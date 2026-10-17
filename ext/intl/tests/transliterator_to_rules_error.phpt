--TEST--
Transliterator::toRules() validates objects and resets previous errors
--EXTENSIONS--
intl
--INI--
intl.error_level=0
--FILE--
<?php
$transliterator = Transliterator::createFromRules('a > b;');
var_dump($transliterator->transliterate("\xff"));
var_dump($transliterator->toRules());
var_dump($transliterator->getErrorCode(), intl_get_error_code());

class UnconstructedTransliterator extends Transliterator {}
$unconstructed = (new ReflectionClass(UnconstructedTransliterator::class))->newInstanceWithoutConstructor();
try {
    $unconstructed->toRules();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(false)
string(6) "a > b;"
int(0)
int(0)
Found unconstructed transliterator
