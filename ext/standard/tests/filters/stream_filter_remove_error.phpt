--TEST--
Test stream_filter_remove() function : error conditions
--SKIPIF--
<?php
$filters = stream_get_filters();
if(! in_array( "string.rot13", $filters )) die( "skip rot13 filter not available." );
?>
--FILE--
<?php
$file = __DIR__ . DIRECTORY_SEPARATOR . 'streamfilterTest.txt';
touch( $file );
$fp = fopen( $file, 'w+' );
$filter = stream_filter_append( $fp, "string.rot13", STREAM_FILTER_WRITE );

echo "*** Testing stream_filter_remove() : error conditions ***\n";

echo "\n-- Testing stream_filter_remove() function with bad resource --\n";
try {
    stream_filter_remove($fp);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

echo "\n-- Testing stream_filter_remove() function with an already removed filter --\n";
// Double remove it
var_dump(stream_filter_remove( $filter ));
try {
    stream_filter_remove($filter);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

fclose( $fp );

?>
--CLEAN--
<?php

$file = __DIR__ . DIRECTORY_SEPARATOR . 'streamfilterTest.txt';
unlink( $file );

?>
--EXPECT--
*** Testing stream_filter_remove() : error conditions ***

-- Testing stream_filter_remove() function with bad resource --
stream_filter_remove(): supplied resource is not a valid stream filter resource

-- Testing stream_filter_remove() function with an already removed filter --
bool(true)
stream_filter_remove(): supplied resource is not a valid stream filter resource
