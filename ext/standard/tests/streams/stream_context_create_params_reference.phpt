--TEST--
stream_context_create() and stream_context_set_params() accept an "options" element held by reference
--FILE--
<?php

$options = ['http' => ['method' => 'HEAD']];
$params = ['options' => &$options];

try {
    $context = stream_context_create(null, $params);
    var_dump(stream_context_get_options($context));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$context = stream_context_create();
try {
    var_dump(stream_context_set_params($context, $params));
    var_dump(stream_context_get_options($context));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$invalid = 'not an array';
try {
    stream_context_create(null, ['options' => &$invalid]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
array(1) {
  ["http"]=>
  array(1) {
    ["method"]=>
    string(4) "HEAD"
  }
}
bool(true)
array(1) {
  ["http"]=>
  array(1) {
    ["method"]=>
    string(4) "HEAD"
  }
}
TypeError: Invalid stream/context parameter
