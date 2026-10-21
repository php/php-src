--TEST--
Serialization contexts abandoned by a bailout are reset before processing the next request
--EXTENSIONS--
session
zend_test
--INI--
allow_url_fopen=1
--ENV--
PHP_CLI_SERVER_WORKERS=
--SKIPIF--
<?php
include "skipif.inc";
if (!function_exists("proc_open")) {
    die("skip proc_open is required");
}
?>
--FILE--
<?php
include "php_cli_server.inc";

$sessionDir = __DIR__ . "/serialization_context_request_reset-sessions-" . getmypid();
mkdir($sessionDir);
register_shutdown_function(function () use ($sessionDir) {
    $path = $sessionDir . "/sess_serialization-reset";
    if (file_exists($path)) {
        unlink($path);
    }
    rmdir($sessionDir);
});
$args = [
    "-d", "session.upload_progress.enabled=1",
    "-d", "session.upload_progress.cleanup=0",
    "-d", "session.upload_progress.freq=0",
    "-d", "session.upload_progress.min_freq=0",
    "-d", "session.use_strict_mode=0",
    "-d", "session.save_handler=files",
    "-d", "session.save_path=$sessionDir",
];
foreach (["session", "zend_test"] as $extension) {
    $path = ini_get("extension_dir") . DIRECTORY_SEPARATOR . $extension . "." . PHP_SHLIB_SUFFIX;
    if (file_exists($path)) {
        $args[] = "-d";
        $args[] = "extension=$path";
    }
}
$server = php_cli_server_start(<<<'PHP'
session_id("serialization-reset");
session_start();
echo getmypid(), "\n";
if (isset($_GET["bailout"])) {
    $_SESSION = ["value" => ["nested" => 42]];
    session_write_close();
    echo "bailout\n";
    zend_test_serialization_bailout((bool) $_GET["unserialize"], (bool) $_GET["locked"]);
}
var_dump(zend_test_serialization_state());
var_dump($_SESSION["value"]);
var_dump($_SESSION["upload_progress_test"]["done"]);
var_dump($_FILES["file"]["error"]);
session_destroy();
PHP, cmd_args: $args);

$body = "--boundary\r\n"
    . "Content-Disposition: form-data; name=\"PHP_SESSION_UPLOAD_PROGRESS\"\r\n\r\n"
    . "test\r\n"
    . "--boundary\r\n"
    . "Content-Disposition: form-data; name=\"file\"; filename=\"test.txt\"\r\n"
    . "Content-Type: text/plain\r\n\r\n"
    . "contents\r\n"
    . "--boundary--\r\n";

foreach ([0, 1] as $unserialize) {
    foreach ([0, 1] as $locked) {
        [$pid, $response] = explode("\n", file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS
            . "/?bailout=1&unserialize=$unserialize&locked=$locked"), 2);
        echo $response;
        $context = stream_context_create(["http" => [
            "method" => "POST",
            "header" => "Content-Type: multipart/form-data; boundary=boundary\r\n"
                . "Cookie: PHPSESSID=serialization-reset\r\n",
            "content" => $body,
        ]]);
        $response = file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS, false, $context);
        if ($response === false) {
            echo file_get_contents($server->outputFile);
            break 2;
        }
        [$nextPid, $response] = explode("\n", $response, 2);
        if ($nextPid !== $pid) {
            throw new RuntimeException("Requests must use the same worker");
        }
        echo $response;
    }
}
?>
--EXPECT--
bailout
array(5) {
  ["lock"]=>
  int(0)
  ["serialize_level"]=>
  int(0)
  ["serialize_data"]=>
  bool(false)
  ["unserialize_level"]=>
  int(0)
  ["unserialize_data"]=>
  bool(false)
}
array(1) {
  ["nested"]=>
  int(42)
}
bool(true)
int(0)
bailout
array(5) {
  ["lock"]=>
  int(0)
  ["serialize_level"]=>
  int(0)
  ["serialize_data"]=>
  bool(false)
  ["unserialize_level"]=>
  int(0)
  ["unserialize_data"]=>
  bool(false)
}
array(1) {
  ["nested"]=>
  int(42)
}
bool(true)
int(0)
bailout
array(5) {
  ["lock"]=>
  int(0)
  ["serialize_level"]=>
  int(0)
  ["serialize_data"]=>
  bool(false)
  ["unserialize_level"]=>
  int(0)
  ["unserialize_data"]=>
  bool(false)
}
array(1) {
  ["nested"]=>
  int(42)
}
bool(true)
int(0)
bailout
array(5) {
  ["lock"]=>
  int(0)
  ["serialize_level"]=>
  int(0)
  ["serialize_data"]=>
  bool(false)
  ["unserialize_level"]=>
  int(0)
  ["unserialize_data"]=>
  bool(false)
}
array(1) {
  ["nested"]=>
  int(42)
}
bool(true)
int(0)
