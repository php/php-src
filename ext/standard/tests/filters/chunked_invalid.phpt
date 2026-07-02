--TEST--
Chunked encoding with invalid values
--SKIPIF--
<?php
$filters = stream_get_filters();
if(! in_array( "dechunk", $filters )) die( "skip Chunked filter not available." );
?>
--INI--
allow_url_fopen=1
--FILE--
<?php
$streams = array(
    "data://text/plain,1\r\n",
    "data://text/plain,1\r\n0\r\n",
    "data://text/plain,1\r\nab\r\n0\r\n",
    "data://text/plain,a\r\n",
    "data://text/plain,z\r\n",
    "data://text/plain,z\r\n0\r\n",
    "data://text/plain,a string that starts with a valid hex character\r\n0\r\n",
    "data://text/plain,some string that does not start with a hex character\r\n0\r\n",
);
foreach ($streams as $name) {
    $fp = fopen($name, "r");
    stream_filter_append($fp, "dechunk", STREAM_FILTER_READ);
    var_dump(stream_get_contents($fp));
    fclose($fp);
}
?>
--EXPECTF--

Warning: stream_get_contents(): Stream filter (dechunk): unexpected end of stream in %s on line %d
string(0) ""

Warning: stream_get_contents(): Stream filter (dechunk): unexpected end of stream in %s on line %d
string(0) ""

Warning: stream_get_contents(): Stream filter (dechunk): unexpected end of stream in %s on line %d
string(0) ""

Warning: stream_get_contents(): Stream filter (dechunk): unexpected end of stream in %s on line %d
string(0) ""

Warning: stream_get_contents(): Stream filter (dechunk): unexpected end of stream in %s on line %d
string(0) ""

Warning: stream_get_contents(): Stream filter (dechunk): unexpected end of stream in %s on line %d
string(0) ""

Warning: stream_get_contents(): Stream filter (dechunk): unexpected end of stream in %s on line %d
string(0) ""

Warning: stream_get_contents(): Stream filter (dechunk): unexpected end of stream in %s on line %d
string(0) ""
