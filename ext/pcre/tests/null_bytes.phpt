--TEST--
Zero byte test
--FILE--
<?php

try {
    preg_match("\0//i", "");
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    preg_match("/\0/i", "");
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    preg_match("//\0i", "");
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    preg_match("//i\0", "");
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    preg_match("/\\\0/i", "");
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    preg_match("\0[]i", "");
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    preg_match("[\0]i", "");
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    preg_match("[]\0i", "");
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    preg_match("[]i\0", "");
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    preg_match("[\\\0]i", "");
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    preg_replace("/foo/e\0/i", "echo('Eek');", "");
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
preg_match(): Regular expression cannot contain any null-bytes
preg_match(): Regular expression cannot contain any null-bytes
preg_match(): Regular expression cannot contain any null-bytes
preg_match(): Regular expression cannot contain any null-bytes
preg_match(): Regular expression cannot contain any null-bytes
preg_match(): Regular expression cannot contain any null-bytes
preg_match(): Regular expression cannot contain any null-bytes
preg_match(): Regular expression cannot contain any null-bytes
preg_match(): Regular expression cannot contain any null-bytes
preg_match(): Regular expression cannot contain any null-bytes
preg_replace(): Regular expression cannot contain any null-bytes
