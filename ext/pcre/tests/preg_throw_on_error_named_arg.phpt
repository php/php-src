--TEST--
PREG_THROW_ON_ERROR: the new preg_replace()/preg_filter() $flags parameter is reachable by name
--FILE--
<?php

try {
    preg_replace('//u', 'x', "\xff", flags: PREG_THROW_ON_ERROR);
} catch (PregException $e) {
    echo 'preg_replace: ', $e->getMessage(), "\n";
}

try {
    preg_filter('//u', 'x', "\xff", flags: PREG_THROW_ON_ERROR);
} catch (PregException $e) {
    echo 'preg_filter: ', $e->getMessage(), "\n";
}

?>
--EXPECT--
preg_replace: Malformed UTF-8 characters, possibly incorrectly encoded
preg_filter: Malformed UTF-8 characters, possibly incorrectly encoded
