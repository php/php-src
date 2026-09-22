--TEST--
mime_content_type(): Testing wrong parameters
--EXTENSIONS--
fileinfo
--FILE--
<?php

try {
    mime_content_type(1);
} catch (\TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    mime_content_type(NULL);
} catch (\TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    mime_content_type(new stdclass);
} catch (\TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    mime_content_type(array());
} catch (\TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

mime_content_type('foo/inexistent');

try {
    mime_content_type('');
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    mime_content_type("\0");
} catch (\TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
TypeError: mime_content_type(): Argument #1 ($filename) must be of type resource|string, int given
TypeError: mime_content_type(): Argument #1 ($filename) must be of type resource|string, null given
TypeError: mime_content_type(): Argument #1 ($filename) must be of type resource|string, stdClass given
TypeError: mime_content_type(): Argument #1 ($filename) must be of type resource|string, array given

Warning: mime_content_type(): Failed to open stream: No such file or directory in %s on line %d
ValueError: mime_content_type(): Argument #1 ($filename) must not be empty
TypeError: mime_content_type(): Argument #1 ($filename) must not contain any null bytes
