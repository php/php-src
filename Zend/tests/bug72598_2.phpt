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
Warning: Parameter 1 to ref() expected to be a reference, value given in %sbug72598_2.php on line 14
int(0)

Warning: Parameter 1 to ref() expected to be a reference, value given in %sbug72598_2.php on line 14
int(0)
