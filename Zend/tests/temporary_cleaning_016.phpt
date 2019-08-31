--TEST--
Live ranges should be ordered according to "start" position
--FILE--
<?php
set_error_handler(function($no, $msg) { throw new Exception; });

try {
    $a = [];
    $str = "$a${"y$a$a"}y";
} catch (Exception $e) {
}
?>
DONE
--EXPECT--
DONE
