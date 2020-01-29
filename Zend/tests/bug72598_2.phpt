--TEST--
Bug #72598.2 (Reference is lost after array_slice())
--FILE--
<?php
function ref(&$ref) {
    var_dump($ref);
    $ref = 1;
}

new class {
        function __construct() {
        $b = 0;
                $args = [&$b];
        unset($b);
                for ($i = 0; $i < 2; $i++) {
                        $a = array_slice($args, 0, 1);
                        call_user_func_array('ref', $a);
                }
        }
};
?>
--EXPECTF--
Warning: ref() expects argument #1 ($ref) to be passed by reference, value given in %s on line %d
int(0)

Warning: ref() expects argument #1 ($ref) to be passed by reference, value given in %s on line %d
int(0)
