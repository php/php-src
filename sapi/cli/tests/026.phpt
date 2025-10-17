--TEST--
CLI php --ini=json
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE');

$interface = shell_exec(sprintf("%s --ini=json", $php));
$script  = shell_exec(sprintf(
    "%s -r 'echo json_encode([
        \"file\"    => php_ini_loaded_file(),
        \"scanned\" => php_ini_scanned_files() ?: []]);'",
    $php));

$response["interface"] = json_decode($interface,
    JSON_THROW_ON_ERROR|JSON_OBJECT_AS_ARRAY);
$response["script"] = json_decode($script, JSON_THROW_ON_ERROR);
if (isset($response["script"]["scanned"]) &&
    $response["script"]["scanned"]) {
    $response["script"]["scanned"] = array_map('trim',
        explode(",\n", $response["script"]["scanned"]));
}

if (!isset($response["interface"]["file"]) &&
     isset($response["script"]["file"]) &&
     $response["script"]["file"]) {
    echo "interface is missing file\n";
    var_dump($response);
}

if (!isset($response["interface"]["scanned"]) &&
     isset($response["script"]["scanned"]) &&
    $response["script"]["scanned"]) {
    echo "interface is missing scanned\n";
    var_dump($response);
}

if (isset($response["interface"]["scanned"]) &&
    $response["interface"]["scanned"] != $response["script"]["scanned"]) {
    echo "interface and script scanned do not match\n";
    var_dump($response);
}

echo "OK\n";
?>
--EXPECT--
OK