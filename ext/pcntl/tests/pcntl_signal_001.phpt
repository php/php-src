--TEST--
pcntl_signal() signal greater than max available.
--EXTENSIONS--
pcntl
--FILE--
<?php
pcntl_signal(1000000, function($signo){
    echo "signaled\n";
});
echo "ok\n";
?>
--EXPECTF--
Fatal error: Uncaught ValueError: pcntl_signal(): Argument #1 ($signal) must be less than %d %s
%A
