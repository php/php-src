--TEST--
ZendMM no-reuse mode rejects invalid settings and incompatible allocators
--SKIPIF--
<?php
if (!function_exists("proc_open")) {
    die("skip proc_open is required");
}
?>
--FILE--
<?php
$env = getenv();
$cases = [
    ["ZEND_MM_NO_REUSE" => "invalid", "USE_ZEND_ALLOC" => "1"],
    ["ZEND_MM_NO_REUSE" => "1", "USE_ZEND_ALLOC" => "0"],
    ["ZEND_MM_NO_REUSE" => "1", "USE_ZEND_ALLOC" => "1", "ZEND_MM_DEBUG" => "poison_free=0"],
];
foreach ($cases as $case) {
    $process = proc_open(
        [PHP_BINARY, "-n", "-r", "echo 'Unexpected startup';"],
        [0 => ["pipe", "r"], 1 => ["pipe", "w"], 2 => ["pipe", "w"]],
        $pipes,
        null,
        array_replace($env, $case)
    );
    fclose($pipes[0]);
    $output = stream_get_contents($pipes[1]);
    $error = stream_get_contents($pipes[2]);
    fclose($pipes[1]);
    fclose($pipes[2]);
    var_dump(proc_close($process) !== 0);
    var_dump($output === "");
    echo strtok($error, "\r\n"), "\n";
}
?>
--EXPECT--
bool(true)
bool(true)
ZEND_MM_NO_REUSE must be 0 or 1
bool(true)
bool(true)
ZEND_MM_NO_REUSE requires native ZendMM (USE_ZEND_ALLOC=1)
bool(true)
bool(true)
ZEND_MM_NO_REUSE does not support custom heap handlers
