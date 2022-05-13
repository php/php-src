--TEST--
Promoting float precision warning to exception in bitwise_not
--FILE--
<?php
set_error_handler(function($_, $msg) {
    throw new Exception($msg);
});
try {
    var_dump(~INF);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Implicit conversion from float INF to int loses precision
