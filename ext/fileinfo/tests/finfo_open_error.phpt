--TEST--
Test finfo_open() function : error functionality
--EXTENSIONS--
fileinfo
--FILE--
<?php
$magicFile = __DIR__ . DIRECTORY_SEPARATOR . 'magic';

echo "*** Testing finfo_open() : error functionality ***\n";

var_dump( finfo_open( FILEINFO_MIME, 'foobarfile' ) );
var_dump( finfo_open( PHP_INT_MAX - 1, $magicFile ) );

try {
    var_dump( finfo_open( 'foobar' ) );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump( new finfo('foobar') );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
*** Testing finfo_open() : error functionality ***

Warning: finfo_open(%sfoobarfile): Failed to open stream: No such file or directory in %sfinfo_open_error.php on line %d

Warning: finfo_open(%sfoobarfile): Failed to open stream: No such file or directory in %sfinfo_open_error.php on line %d

Warning: finfo_open(): Failed to load magic database at "%sfoobarfile" in %sfinfo_open_error.php on line %d
bool(false)

Warning: finfo_open(): using regular magic file `%smagic' in %sfinfo_open_error.php on line %d
object(finfo)#%d (0) {
}
finfo_open(): Argument #1 ($flags) must be of type int, string given
finfo::__construct(): Argument #1 ($flags) must be of type int, string given
