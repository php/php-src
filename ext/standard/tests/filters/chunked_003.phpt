--TEST--
Chunked encoding (error handling)
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
    "data://text/plain,apple",
    "data://text/plain,mango",
);
foreach ($streams as $name) {
    $fp = fopen($name, "r");
    stream_filter_append($fp, "dechunk", STREAM_FILTER_READ);
    var_dump(stream_get_contents($fp));
    fclose($fp);
}
?>
--EXPECT--
string(5) "apple"
string(5) "mango"
