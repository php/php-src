--TEST--
Crash when function parameter modified via unexpected reference
--FILE--
<?php
class Test {
    public function __toString() {
        global $my_var;
        $my_var = 0;
        return ",";
    }
}
$my_var = str_repeat("A",64);
$data = call_user_func_array("explode",array(new Test(), &$my_var));
$my_var=array(1,2,3);
$data = call_user_func_array("implode",array(&$my_var, new Test()));
echo "Done.\n";
?>
--EXPECTF--
Deprecated: implode(): Passing glue string after array is deprecated. Swap the parameters in %s on line %d
Done.
