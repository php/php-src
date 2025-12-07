--TEST--
sscanf(): with strings containing null bytes
--FILE--
<?php

$str = "Hello\0wonderful\0World!";
$format = "%s\0%s\0%s";
try {
    var_dump(sscanf($str, $format));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump(sscanf($str, $format, $a, $b, $c));
    var_dump($a, $b, $c);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
array(3) {
  [0]=>
  string(5) "Hello"
  [1]=>
  NULL
  [2]=>
  NULL
}
int(1)
string(5) "Hello"
NULL
NULL
