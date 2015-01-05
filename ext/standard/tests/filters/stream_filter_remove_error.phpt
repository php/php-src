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

$file = dirname( __FILE__ ) . DIRECTORY_SEPARATOR . 'streamfilterTest.txt';
touch( $file );
$fp = fopen( $file, 'w+' );
$filter = stream_filter_append( $fp, "string.rot13", STREAM_FILTER_WRITE );

echo "*** Testing stream_filter_remove() : error conditions ***\n";

echo "\n-- Testing stream_filter_remove() function with Zero arguments --\n";
var_dump( stream_filter_remove() );

echo "\n-- Testing stream_filter_remove() function with more than expected no. of arguments --\n";
$arg = 'bogus arg';
var_dump( stream_filter_remove( $filter, $arg ) );

echo "\n-- Testing stream_filter_remove() function with unexisting stream filter --\n";
var_dump( stream_filter_remove( "fakefilter" ) );

echo "\n-- Testing stream_filter_remove() function with bad resource --\n";
var_dump( stream_filter_remove( $fp ) );

echo "\n-- Testing stream_filter_remove() function with an already removed filter --\n";
// Double remove it
var_dump( stream_filter_remove( $filter ) );
var_dump( stream_filter_remove( $filter ) );

fclose( $fp );

?>
===DONE===
--CLEAN--
<?php

$file = dirname( __FILE__ ) . DIRECTORY_SEPARATOR . 'streamfilterTest.txt';
unlink( $file );

?>
--EXPECTF--
*** Testing stream_filter_remove() : error conditions ***

-- Testing stream_filter_remove() function with Zero arguments --

Warning: stream_filter_remove() expects exactly 1 parameter, 0 given in %s on line %d
bool(false)

-- Testing stream_filter_remove() function with more than expected no. of arguments --

Warning: stream_filter_remove() expects exactly 1 parameter, 2 given in %s on line %d
bool(false)

-- Testing stream_filter_remove() function with unexisting stream filter --

Warning: stream_filter_remove() expects parameter 1 to be resource, string given in %s on line %d
bool(false)

-- Testing stream_filter_remove() function with bad resource --

Warning: stream_filter_remove(): Invalid resource given, not a stream filter in %s on line %d
bool(false)

-- Testing stream_filter_remove() function with an already removed filter --
bool(true)

Warning: stream_filter_remove(): Invalid resource given, not a stream filter in %s on line %d
bool(false)
===DONE===
