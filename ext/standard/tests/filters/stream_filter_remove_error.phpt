--TEST--
Test stream_filter_remove() function : error conditions
--SKIPIF--
<?php
$filters = stream_get_filters();
if(! in_array( "string.rot13", $filters )) die( "skip rot13 filter not available." );
?>
--FILE--
<?php
/* Prototype  : bool stream_filter_remove(resource stream_filter)
 * Description: Flushes any data in the filter's internal buffer, removes it from the chain, and frees the resource
 * Source code: ext/standard/streamsfuncs.c
 * Alias to functions:
 */

$file = __DIR__ . DIRECTORY_SEPARATOR . 'streamfilterTest.txt';
touch( $file );
$fp = fopen( $file, 'w+' );
$filter = stream_filter_append( $fp, "string.rot13", STREAM_FILTER_WRITE );

echo "*** Testing stream_filter_remove() : error conditions ***\n";

echo "\n-- Testing stream_filter_remove() function with bad resource --\n";
var_dump( stream_filter_remove( $fp ) );

echo "\n-- Testing stream_filter_remove() function with an already removed filter --\n";
// Double remove it
var_dump( stream_filter_remove( $filter ) );
var_dump( stream_filter_remove( $filter ) );

fclose( $fp );

?>
--CLEAN--
<?php

$file = __DIR__ . DIRECTORY_SEPARATOR . 'streamfilterTest.txt';
unlink( $file );

?>
--EXPECTF--
*** Testing stream_filter_remove() : error conditions ***

-- Testing stream_filter_remove() function with bad resource --

Warning: stream_filter_remove(): Invalid resource given, not a stream filter in %s on line %d
bool(false)

-- Testing stream_filter_remove() function with an already removed filter --
bool(true)

Warning: stream_filter_remove(): Invalid resource given, not a stream filter in %s on line %d
bool(false)
