--TEST--
Test posix_ttyname() with wrong parameters
--DESCRIPTION--
Gets the absolute path to the current terminal device that is open on a given file descriptor.
Source code: ext/posix/posix.c

--CREDITS--
Falko Menge, mail at falko-menge dot de
PHP Testfest Berlin 2009-05-10
--EXTENSIONS--
posix
standard
--FILE--
<?php

var_dump(posix_ttyname(0)); // param not a resource

$bucket = stream_bucket_new(fopen('php://temp', 'w+'), '');

try {
    var_dump(posix_ttyname($bucket->bucket)); // wrong resource type
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(false)
posix_ttyname(): supplied resource is not a valid stream resource
