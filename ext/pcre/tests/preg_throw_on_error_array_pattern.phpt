--TEST--
PREG_THROW_ON_ERROR: preg_replace() with an array of patterns throws on the first failing pattern
--FILE--
<?php

try {
    preg_replace(['/a/', '/[/', '/c/'], 'x', 'abc', -1, $count, PREG_THROW_ON_ERROR);
    echo "compile: no exception thrown\n";
} catch (PregException $e) {
    printf("compile: %s | matches: %s\n", $e->getMessage(),
        ($e->getCode() === preg_last_error() && $e->getMessage() === preg_last_error_msg()) ? 'yes' : 'no');
}

try {
    preg_replace(['/a/', '//u'], 'x', "\xff", -1, $count, PREG_THROW_ON_ERROR);
    echo "exec: no exception thrown\n";
} catch (PregException $e) {
    printf("exec: %s | matches: %s\n", $e->getMessage(),
        ($e->getCode() === preg_last_error() && $e->getMessage() === preg_last_error_msg()) ? 'yes' : 'no');
}

?>
--EXPECT--
compile: Internal error | matches: yes
exec: Malformed UTF-8 characters, possibly incorrectly encoded | matches: yes
