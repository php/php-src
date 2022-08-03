--TEST--
pcntl_signal() signal greater than max available.
--EXTENSIONS--
pcntl
--FILE--
<?php
try {
    pcntl_signal(1000000, function($signo){
        echo "signaled\n";
    });    
} catch (Error $e) {
    echo $e->getMessage();
}
?>
--EXPECTF--
pcntl_signal(): Argument #1 ($signal) must be less than %d
