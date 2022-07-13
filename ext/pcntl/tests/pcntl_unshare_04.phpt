--TEST--
pcntl_unshare() - Error with wrong flag. Test return value type for each valid flag.
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (!function_exists("pcntl_unshare")) die("skip pcntl_unshare is not available");
?>
--FILE--
<?php

$result = null;

try {
    $result = pcntl_unshare(-1);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

var_dump($result);

foreach ([CLONE_NEWNS, CLONE_NEWIPC , CLONE_NEWUTS , CLONE_NEWNET, CLONE_NEWPID ,CLONE_NEWUSER, CLONE_NEWCGROUP] as $flag) {
    $result = @pcntl_unshare($flag);
    var_dump($result);
}

?>
--EXPECTF--
pcntl_unshare(): Argument #1 ($flags) must be a combination of CLONE_* flags
NULL
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
