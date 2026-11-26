--TEST--
sodium_add() and sodium_increment() reset the cached hash and the UTF-8 flag of the string they write
--EXTENSIONS--
sodium
--FILE--
<?php

$x = 'c';
$array = ['abc' => 1, 'bbc' => 2];

foreach (['sodium_add', 'sodium_increment'] as $function) {
    // The hash of "abc" is cached by the lookup, then the bytes change
    $string = 'ab' . $x;
    var_dump(isset($array[$string]));
    $function === 'sodium_add' ? sodium_add($string, "\x01\x00\x00") : sodium_increment($string);
    var_dump($string === 'bbc', isset($array[$string]));
}

// The valid UTF-8 flag is set by the match, then the bytes become invalid
$string = 'ab' . $x;
var_dump(preg_match('/./u', $string));
sodium_add($string, "\x00\x00\x8e");
var_dump(bin2hex($string), preg_match('/./u', $string));

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
int(1)
string(6) "6162f1"
bool(false)
