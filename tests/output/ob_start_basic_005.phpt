--TEST--
ob_start(): non-static method as static callbacks.
--FILE--
<?php
/*
 * Function is implemented in main/output.c
*/

Class C {
    function h($string) {
        return $string;
    }
}

function checkAndClean() {
  var_dump(ob_list_handlers());
  while (ob_get_level()>0) {
    ob_end_flush();
  }
}

try {
    var_dump(ob_start('C::h'));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
checkAndClean();

?>
--EXPECT--
TypeError: ob_start(): Argument #1 ($callback) must be a valid callback or null, non-static method C::h() cannot be called statically
array(0) {
}
