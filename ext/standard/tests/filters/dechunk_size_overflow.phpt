--TEST--
dechunk filter must reject a chunk size that overflows size_t
--SKIPIF--
<?php
$filters = stream_get_filters();
if(! in_array( "dechunk", $filters )) die( "skip Chunked filter not available." );
?>
--INI--
allow_url_fopen=1
--FILE--
<?php
/* Both sizes exceed SIZE_MAX, so parsing stops and the rest is passed through
   raw. The guard trips at SIZE_MAX/16, so how many digits are consumed first
   follows the width of size_t: %s covers the leftover run. Unguarded, the
   first size wraps to 0, which reads as the terminating chunk and drops the
   body; the second wraps to SIZE_MAX and swallows the rest as one chunk. */
$streams = [
    "data://text/plain,10000000000000000\nBODYDATA\n0\n",
    "data://text/plain,fffffffffffffffff\nBODYDATA\n0\n",
    "data://text/plain,5\nhello\n0\n",
];
foreach ($streams as $name) {
    $fp = fopen($name, "r");
    stream_filter_append($fp, "dechunk", STREAM_FILTER_READ);
    var_dump(stream_get_contents($fp));
    fclose($fp);
}
?>
--EXPECTF--
string(%d) "%s
BODYDATA
0
"
string(%d) "%s
BODYDATA
0
"
string(5) "hello"
