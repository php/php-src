--TEST--
GH-25756 (OPcache: "Invalid opcode 62/4/0" when assigning a reference in a return)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php

function test_assign_ref() {
    $b = 'success';
    return $a = &$b;
}

function &get_ref() {
    static $x = 'from_func';
    return $x;
}

function test_assign_func_ref() {
    return $a = &get_ref();
}

function test_assign_obj_ref(stdClass $obj, &$b) {
    return $obj->p = &$b;
}

function test_assign_dim_ref() {
    $b = 'dim_val';
    $arr = [];
    return $arr[0] = &$b;
}

class StaticPropHolder {
    public static $p;
}

function test_assign_static_prop_ref(&$b) {
    return StaticPropHolder::$p = &$b;
}

function test_list_by_ref() {
    $arr = ['list_val'];
    return [&$a] = $arr;
}

function test_ternary_assign_ref(bool $cond) {
    $b = 'ternary_val';
    return $cond ? ($a = &$b) : 'other';
}

function test_match_assign_ref(bool $cond) {
    $b = 'match_val';
    return match ($cond) {
        true => $a = &$b,
        default => 'other',
    };
}

function test_typed_assign_ref(): string {
    $b = 'typed_val';
    return $a = &$b;
}

$obj = new stdClass();
$obj_src = 'obj_prop';
$obj_ret = test_assign_obj_ref($obj, $obj_src);
$obj_src = 'obj_changed';

$static_src = 'static_prop';
$static_ret = test_assign_static_prop_ref($static_src);
$static_src = 'static_changed';

$arrow_src = 'arrow_val';
$arrow = fn() => $a = &$arrow_src;

echo "1: ", test_assign_ref(), "\n";
echo "2: ", test_assign_func_ref(), "\n";
echo "3: ", $obj_ret, " / ", $obj->p, "\n";
echo "4: ", test_assign_dim_ref(), "\n";
echo "5: ", $static_ret, " / ", StaticPropHolder::$p, "\n";
echo "6: ", implode(',', test_list_by_ref()), "\n";
echo "7: ", test_ternary_assign_ref(true), "\n";
echo "8: ", test_match_assign_ref(true), "\n";
echo "9: ", test_typed_assign_ref(), "\n";
echo "10: ", $arrow(), "\n";
?>
--EXPECT--
1: success
2: from_func
3: obj_prop / obj_changed
4: dim_val
5: static_prop / static_changed
6: list_val
7: ternary_val
8: match_val
9: typed_val
10: arrow_val
