--TEST--
The length-mismatch errors name the real second parameter
--EXTENSIONS--
sodium
--FILE--
<?php

$short = str_repeat("\x01", 4);
$long = str_repeat("\x01", 8);

foreach (['sodium_add', 'sodium_memcmp', 'sodium_compare'] as $function) {
    try {
        $first = $short;
        $function($first, $long);
    } catch (SodiumException $e) {
        echo $e->getMessage(), "\n";
    }
}

/* the messages name argument #2, so that name has to be the real one */
foreach ((new ReflectionFunction('sodium_add'))->getParameters() as $parameter) {
    echo '$', $parameter->getName(), "\n";
}

?>
--EXPECT--
sodium_add(): Argument #1 ($string1) and argument #2 ($string2) must have the same length
sodium_memcmp(): Argument #1 ($string1) and argument #2 ($string2) must have the same length
sodium_compare(): Argument #1 ($string1) and argument #2 ($string2) must have the same length
$string1
$string2
