--TEST--
Delimiters crash test
--FILE--
<?php

try {
    preg_match('', '');
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    preg_match('      ', '');
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

var_dump(preg_match('@@', ''));

try {
    preg_match('12', '');
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

var_dump(preg_match('<>', ''));

try {
    preg_match('~a', '');
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

var_dump(preg_match('@\@\@@', '@@'));

try {
    preg_match('//z', '@@');
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    preg_match('{', '');
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
preg_match(): Regular expression cannot be empty
preg_match(): Regular expression cannot be empty
int(1)
preg_match(): Regular expression delimiter cannot be alphanumeric or a backslash
int(1)
preg_match(): Regular expression doesn't contain an ending delimiter "~"
int(1)
preg_match(): Regular expression modifier "z" is invalid
preg_match(): No ending matching delimiter "}" found
