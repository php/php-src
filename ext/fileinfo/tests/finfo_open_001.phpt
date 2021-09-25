--TEST--
finfo_open(): Testing magic_file names
--EXTENSIONS--
fileinfo
--FILE--
<?php

try {
    var_dump(finfo_open(FILEINFO_MIME, "\0"));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

var_dump(finfo_open(FILEINFO_MIME, NULL));
var_dump(finfo_open(FILEINFO_MIME, ''));
var_dump(finfo_open(FILEINFO_MIME, 123));
var_dump(finfo_open(FILEINFO_MIME, 1.0));
var_dump(finfo_open(FILEINFO_MIME, '/foo/bar/inexistent'));

?>
--EXPECTF--
finfo_open(): Argument #2 ($magic_database) must not contain any null bytes
object(finfo)#%d (0) {
}
object(finfo)#%d (0) {
}

Warning: finfo_open(%s123): Failed to open stream: No such file or directory in %s on line %d

Warning: finfo_open(%s123): Failed to open stream: No such file or directory in %s on line %d

Warning: finfo_open(): Failed to load magic database at "%s123" in %s on line %d
bool(false)

Warning: finfo_open(%s1): Failed to open stream: No such file or directory in %s on line %d

Warning: finfo_open(%s1): Failed to open stream: No such file or directory in %s on line %d

Warning: finfo_open(): Failed to load magic database at "%s1" in %s on line %d
bool(false)

Warning: finfo_open(%sinexistent): Failed to open stream: No such file or directory in %s on line %d

Warning: finfo_open(%sinexistent): Failed to open stream: No such file or directory in %s on line %d

Warning: finfo_open(): Failed to load magic database at "%sinexistent" in %s on line %d
bool(false)
