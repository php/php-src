--TEST--
PREG_THROW_ON_ERROR: exhausting the backtrack limit throws PregException
--INI--
pcre.backtrack_limit=1000
pcre.jit=0
--FILE--
<?php

try {
    preg_match('/(a+)+$/', str_repeat('a', 1000) . 'X', $m, PREG_THROW_ON_ERROR);
} catch (PregException $e) {
    echo $e->getMessage(), "\n";
    var_dump($e->getCode() === PREG_BACKTRACK_LIMIT_ERROR);
}

?>
--EXPECT--
Backtrack limit exhausted
bool(true)
