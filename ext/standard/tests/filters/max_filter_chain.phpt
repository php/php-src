--TEST--
At most 16 filters can be chained in one stream
--EXTENSIONS--
filter
--FILE--
<?php

function createFilterChains($n, $resource) {
    $filter = 'string.toupper';
    $pipes = 'php://filter/' . implode('|', array_fill(0, $n, $filter)) . "/resource=$resource";
    $slashes = 'php://filter/' . implode('/', array_fill(0, $n, $filter)) . "/resource=$resource";
    $resources = str_repeat("php://filter/$filter/resource=", $n) . $resource;
    return [$pipes, $slashes, $resources];
}

$allowed_read = createFilterChains(16, 'data:text/plain,sixteen');
foreach ($allowed_read as $chain) {
    var_dump(file_get_contents($chain));
}

$allowed_include = createFilterChains(16, 'php://temp');
foreach ($allowed_include as $chain) {
    var_dump(include $chain);
}

$blocked_read = createFilterChains(17, 'data:text/plain,seventeen');
foreach ($blocked_read as $chain) {
    var_dump(file_get_contents($chain));
}

$blocked_include = createFilterChains(17, 'php://temp');
foreach ($blocked_include as $chain) {
    var_dump(include $chain);
}

$ctx = stream_context_create(['filter' => ['max_filter_count' => 2]]);
$blocked_read = createFilterChains(3, 'data:text/plain,three');
foreach ($blocked_read as $chain) {
    var_dump(file_get_contents($chain, false, $ctx));
}

$ctx = stream_context_create(['filter' => ['max_filter_count' => 20]]);
$allowed_read = createFilterChains(19, 'data:text/plain,nineteen');
foreach ($allowed_read as $chain) {
    var_dump(file_get_contents($chain, false, $ctx));
}

// Test that the warning is only given once, even when we add two filters over the limit.
$blocked_read = createFilterChains(18, 'data:text/plain,eighteen');
foreach ($blocked_read as $chain) {
    var_dump(file_get_contents($chain));
}

// many filters with stream_filter_append still works
$fp = fopen('data:text/plain,stream_filter_append', 'r');
for ($i = 0; $i < 80; $i++) {
    stream_filter_append($fp, 'string.toupper');
}
var_dump(fread($fp, 30));
fclose($fp);

?>
--EXPECTF--
string(7) "SIXTEEN"
string(7) "SIXTEEN"
string(7) "SIXTEEN"
int(1)
int(1)
int(1)

Deprecated: Using more than 16 filters in a php://filter URL is deprecated, set this limit using the stream context option max_filter_count, or use stream_filter_append in %smax_filter_chain.php on line %d
string(9) "SEVENTEEN"

Deprecated: Using more than 16 filters in a php://filter URL is deprecated, set this limit using the stream context option max_filter_count, or use stream_filter_append in %smax_filter_chain.php on line %d
string(9) "SEVENTEEN"

Deprecated: Using more than 16 filters in a php://filter URL is deprecated, set this limit using the stream context option max_filter_count, or use stream_filter_append in %smax_filter_chain.php on line %d
string(9) "SEVENTEEN"

Deprecated: Using more than 16 filters in a php://filter URL is deprecated, set this limit using the stream context option max_filter_count, or use stream_filter_append in %smax_filter_chain.php on line %d
int(1)

Deprecated: Using more than 16 filters in a php://filter URL is deprecated, set this limit using the stream context option max_filter_count, or use stream_filter_append in %smax_filter_chain.php on line %d
int(1)

Deprecated: Using more than 16 filters in a php://filter URL is deprecated, set this limit using the stream context option max_filter_count, or use stream_filter_append in %smax_filter_chain.php on line %d
int(1)

Warning: file_get_contents(php://filter/string.toupper|string.toupper|string.toupper/resource=data:text/plain,three): Failed to open stream: too many filters in %s on line %d
bool(false)

Warning: file_get_contents(php://filter/string.toupper/string.toupper/string.toupper/resource=data:text/plain,three): Failed to open stream: too many filters in %s on line %d
bool(false)

Warning: file_get_contents(php://filter/string.toupper/resource=php://filter/string.toupper/resource=php://filter/string.toupper/resource=data:text/plain,three): Failed to open stream: too many filters in %s on line %d
bool(false)
string(8) "NINETEEN"
string(8) "NINETEEN"
string(8) "NINETEEN"

Deprecated: Using more than 16 filters in a php://filter URL is deprecated, set this limit using the stream context option max_filter_count, or use stream_filter_append in %smax_filter_chain.php on line %d
string(8) "EIGHTEEN"

Deprecated: Using more than 16 filters in a php://filter URL is deprecated, set this limit using the stream context option max_filter_count, or use stream_filter_append in %smax_filter_chain.php on line %d
string(8) "EIGHTEEN"

Deprecated: Using more than 16 filters in a php://filter URL is deprecated, set this limit using the stream context option max_filter_count, or use stream_filter_append in %smax_filter_chain.php on line %d
string(8) "EIGHTEEN"
string(20) "STREAM_FILTER_APPEND"
