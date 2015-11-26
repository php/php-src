--TEST--
Test stream_filter_remove() function : basic functionality 
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

echo "*** Testing stream_filter_remove() : basic functionality ***\n";

$file = dirname( __FILE__ ) . DIRECTORY_SEPARATOR . 'streamfilterTest.txt';
touch( $file );

$fp = fopen( $file, 'w+' );

$filter = stream_filter_append( $fp, "string.rot13", STREAM_FILTER_WRITE );
fwrite( $fp, "Testing the rot13 filter which shifts some things around." );

var_dump( stream_filter_remove( $filter ) );
fwrite( $fp, "\nadd some more un-filtered foobar\n" );

rewind( $fp );
fpassthru( $fp );
fclose( $fp );

?>
===DONE===
--CLEAN--
<?php

$file = dirname( __FILE__ ) . DIRECTORY_SEPARATOR . 'streamfilterTest.txt';
unlink( $file );

?>
--EXPECTF--
*** Testing stream_filter_remove() : basic functionality ***
bool(true)
Grfgvat gur ebg13 svygre juvpu fuvsgf fbzr guvatf nebhaq.
add some more un-filtered foobar
===DONE===
