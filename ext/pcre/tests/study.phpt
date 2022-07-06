--TEST--
Study regex
--FILE--
<?php

var_dump(preg_match('/(?:(?:(?:(?:(?:(.))))))/  S', 'aeiou', $dump));
var_dump($dump[1]);
var_dump(preg_match('/(?:(?:(?:(?:(?:(.))))))/', 'aeiou', $dump));
var_dump($dump[1]);

var_dump(preg_match('/(?>..)((?:(?>.)|.|.|.|u))/S', 'aeiou', $dump));
var_dump($dump[1]);

// try to trigger usual "match known text" optimization
var_dump(preg_match('/^aeiou$/S', 'aeiou', $dump));
var_dump($dump[0]);
var_dump(preg_match('/aeiou/S', 'aeiou', $dump));
var_dump($dump[0]);

?>
--EXPECT--
int(1)
string(1) "a"
int(1)
string(1) "a"
int(1)
string(1) "i"
int(1)
string(5) "aeiou"
int(1)
string(5) "aeiou"
