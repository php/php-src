--TEST--
Bug #61115: Stream related segfault on fatal error in php_stream_context_del_link.
--FILE--
<?php

$arrayLarge = array_fill(0, 113663, '*');

$resourceFileTemp = fopen('php://temp', 'r+');
stream_context_set_params($resourceFileTemp, array());
try {
    preg_replace('', function() {}, $resourceFileTemp);
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
preg_replace(): Argument #2 ($replacement) must be of type array|string, Closure given
