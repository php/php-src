--TEST--
Str\Matcher and Str\Replacer compare by object identity
--EXTENSIONS--
strmatch
--FILE--
<?php
foreach ([Str\Matcher::class, Str\Replacer::class] as $class) {
    $dictionary = $class === Str\Matcher::class ? ['a'] : ['a' => 'A'];
    $otherDictionary = $class === Str\Matcher::class ? ['b'] : ['a' => 'B'];
    $first = new $class($dictionary);
    $sameDictionary = new $class($dictionary);
    $differentDictionary = new $class($otherDictionary);
    var_dump($first == $first);
    var_dump($first == $sameDictionary);
    var_dump($first == $differentDictionary);
    var_dump(in_array($sameDictionary, [$first]));
    var_dump(in_array($first, [$first]));
    var_dump(array_search($differentDictionary, [$first]));
    var_dump(array_search($first, [$first]));
}
$first = (new Str\Matcher(['a']))->matchAll('a')[0];
$second = (new Str\Matcher(['a']))->matchAll('a')[0];
var_dump($first == $second);
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
int(0)
bool(true)
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
int(0)
bool(true)
