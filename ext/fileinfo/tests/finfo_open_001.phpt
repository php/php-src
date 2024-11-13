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

var_dump($obj = new finfo(FILEINFO_MIME, NULL));
var_dump($obj = new finfo(FILEINFO_MIME, ''));

try {
    var_dump($obj = new finfo(FILEINFO_MIME, 123));
} catch (\Exception $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    var_dump($obj = new finfo(FILEINFO_MIME, 1.0));
} catch (\Exception $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    var_dump($obj = new finfo(FILEINFO_MIME, '/foo/bar/inexistent'));
} catch (\Exception $e) {
    echo $e->getMessage() . PHP_EOL;
}
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
object(finfo)#%d (%d) {
}
object(finfo)#%d (%d) {
}
finfo::__construct(%s123): Failed to open stream: No such file or directory
finfo::__construct(%s1): Failed to open stream: No such file or directory
finfo::__construct(%sinexistent): Failed to open stream: No such file or directory
