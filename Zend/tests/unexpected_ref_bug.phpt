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
$my_var = str_repeat("A", 64);
$data = call_user_func_array("explode", array(new Test(), &$my_var));
$my_var = str_repeat("A", 64);
$data = call_user_func_array("str_replace", array(&$my_var, new Test(), "foo"));
echo "Done.\n";
?>
--EXPECT--
Done.
