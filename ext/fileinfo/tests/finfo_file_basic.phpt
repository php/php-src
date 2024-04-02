--TEST--
Test finfo_file() function : basic functionality
--EXTENSIONS--
fileinfo
--FILE--
<?php
$magicFile = __DIR__ . DIRECTORY_SEPARATOR . 'magic';
$finfo = finfo_open( FILEINFO_MIME );

echo "*** Testing finfo_file() : basic functionality ***\n";

// Calling finfo_file() with all possible arguments
var_dump( finfo_file( $finfo, __FILE__) );
var_dump( finfo_file( $finfo, __FILE__, FILEINFO_CONTINUE ) );
var_dump( finfo_file( $finfo, $magicFile ) );
try {
    var_dump( finfo_file( $finfo, $magicFile.chr(0).$magicFile) );
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
*** Testing finfo_file() : basic functionality ***
string(28) "text/x-php; charset=us-ascii"
string(22) "PHP script, ASCII text"
string(29) "text/x-file; charset=us-ascii"
finfo_file(): Argument #1 ($finfo) must not contain any null bytes
