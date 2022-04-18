--TEST--
Magic object handlers segfaults and memory errors
--FILE--
<?php
function f($x) {
    return $x;
}

class foo implements ArrayAccess {
    function __get($property) {
        $GLOBALS["y"] = $property;
    }
    function __set($property, $value) {
        $GLOBALS["y"] = $property;
    }
    function __call($func, $args) {
        $GLOBALS["y"] = $func;
    }
    static function __callStatic($func, $args) {
        $GLOBALS["y"] = $func;
    }
    function offsetGet($index): mixed {
        $GLOBALS["y"] = $index;
        return null;
    }
    function offsetSet($index, $newval): void {
        $GLOBALS["y"] = $index;
    }
    function offsetExists($index): bool {
        $GLOBALS["y"] = $index;
        return true;
    }
    function offsetUnset($index): void {
        $GLOBALS["y"] = $index;
    }
}

$x = new foo();
$y = null;

// IS_CONST
$z = $x->const_get;
echo $y,"\n";
$x->const_set = 1;
echo $y,"\n";
$x->const_call();
echo $y,"\n";
foo::const_callstatic();
echo $y,"\n";
$z = $x["const_dim_get"];
echo $y,"\n";
$x["const_dim_set"] = 1;
echo $y,"\n";
isset($x["const_dim_isset"]);
echo $y,"\n";
unset($x["const_dim_unset"]);
echo $y,"\n";

// IS_CONST + conversion
$z = $x->{1};
echo $y,"\n";
$x->{2} = 1;
echo $y,"\n";

// IS_TMP_VAR
$c = "tmp";
$z = $x->{$c."_get"};
echo $y,"\n";
$x->{$c."_set"} = 1;
echo $y,"\n";
$x->{$c."_call"}();
echo $y,"\n";
$z = $x[$c."_dim_get"];
echo $y,"\n";
$x[$c."_dim_set"] = 1;
echo $y,"\n";
isset($x[$c."_dim_isset"]);
echo $y,"\n";
unset($x[$c."_dim_unset"]);
echo $y,"\n";

// IS_TMP_VAR + conversion
$c = 0;
$z = $x->{$c+3};
echo $y,"\n";
$x->{$c+4} = 1;
echo $y,"\n";

// IS_CV
$c = "cv_get";
$z = $x->{$c};
echo $y,"\n";
$c = "cv_set";
$x->{$c} = 1;
echo $y,"\n";
$c = "cv_call";
$x->{$c}();
echo $y,"\n";
$c = "cv_dim_get";
$z = $x[$c];
echo $y,"\n";
$c = "cv_dim_set";
$x[$c] = 1;
echo $y,"\n";
$c = "cv_dim_isset";
isset($x[$c]);
echo $y,"\n";
$c = "cv_dim_unset";
unset($x[$c]);
echo $y,"\n";

// IS_CV + conversion
$c = 5;
$z = $x->{$c};
echo $y,"\n";
$c = 6;
$x->{$c} = 1;
echo $y,"\n";

// IS_VAR
$z = $x->{f("var_get")};
echo $y,"\n";
$x->{f("var_set")} = 1;
echo $y,"\n";
$x->{f("var_call")}();
echo $y,"\n";
$z = $x[f("var_dim_get")];
echo $y,"\n";
$x[f("var_dim_set")] = 1;
echo $y,"\n";
isset($x[f("var_dim_isset")]);
echo $y,"\n";
unset($x[f("var_dim_unset")]);
echo $y,"\n";

// IS_VAR + conversion
$z = $x->{f(7)};
echo $y,"\n";
$x->{f(8)} = 1;
echo $y,"\n";
?>
--EXPECT--
const_get
const_set
const_call
const_callstatic
const_dim_get
const_dim_set
const_dim_isset
const_dim_unset
1
2
tmp_get
tmp_set
tmp_call
tmp_dim_get
tmp_dim_set
tmp_dim_isset
tmp_dim_unset
3
4
cv_get
cv_set
cv_call
cv_dim_get
cv_dim_set
cv_dim_isset
cv_dim_unset
5
6
var_get
var_set
var_call
var_dim_get
var_dim_set
var_dim_isset
var_dim_unset
7
8
