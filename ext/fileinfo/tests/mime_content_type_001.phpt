--TEST--
mime_content_type(): Testing wrong parameters
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc'); ?>
--FILE--
<?php

try {
    mime_content_type(1);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    mime_content_type(NULL);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    mime_content_type(new stdclass);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    mime_content_type(array());
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

mime_content_type('foo/inexistent');

try {
    mime_content_type('');
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    mime_content_type("\0");
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECTF--
mime_content_type(): Argument #1 ($filename) must be of type resource|string, int given
mime_content_type(): Argument #1 ($filename) must be of type resource|string, null given
mime_content_type(): Argument #1 ($filename) must be of type resource|string, stdClass given
mime_content_type(): Argument #1 ($filename) must be of type resource|string, array given

Warning: mime_content_type(foo/inexistent): Failed to open stream: No such file or directory in %s on line %d
mime_content_type(): Argument #1 ($filename) cannot be empty
mime_content_type(): Argument #1 ($filename) must not contain any null bytes
