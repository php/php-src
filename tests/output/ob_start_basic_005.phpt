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
  print_r(ob_list_handlers());
  while (ob_get_level()>0) {
    ob_end_flush();
  }
}

var_dump(ob_start('C::h'));
checkAndClean();

?>
--EXPECTF--
Warning: ob_start(): non-static method C::h() cannot be called statically in %s on line %d

Notice: ob_start(): Failed to create buffer in %s on line %d
bool(false)
Array
(
)
