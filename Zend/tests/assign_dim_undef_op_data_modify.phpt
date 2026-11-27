--TEST--
ASSIGN_DIM: error handler modifies the containers while reporting undefined OP_DATA
--FILE--
<?php
class C { public int $t = 1; }

function grow_outer() {
    global $x;
    set_error_handler(function ($_, $msg) {
        echo $msg, "\n";
        for ($i = 0; $i < 100; $i++) {
            $GLOBALS['x'][] = $i;
        }
    });
    $x = [[1]];
    $x[0][] = $undef;
    var_dump(count($x), $x[0]);
}

function copy_and_replace() {
    global $x;
    set_error_handler(function ($_, $msg) {
        echo $msg, "\n";
        $GLOBALS['keep'] = $GLOBALS['x'];
        $GLOBALS['x'] = 'str';
    });
    $x = [1];
    $x[] = $undef;
    var_dump($x);
}

function copy_only() {
    global $x;
    set_error_handler(function ($_, $msg) {
        echo $msg, "\n";
        $GLOBALS['keep'] = $GLOBALS['x'];
    });
    $x = [1];
    $x[0] = $undef;
    $x = [1];
    $x[] = $undef;
    $x = [1];
    $x[0] .= $undef;
    echo "ok\n";
}

function typed_ref() {
    global $x, $o;
    set_error_handler(function ($_, $msg) {
        echo $msg, "\n";
        $GLOBALS['x'] = 1;
        $GLOBALS['o'] = null;
    });
    $o = new C;
    $x = [0];
    $x[0] = &$o->t;
    $x[0] = $undef;
    var_dump($x, $o);
}

grow_outer();
copy_and_replace();
copy_only();
typed_ref();
?>
--EXPECT--
Undefined variable $undef
int(101)
array(2) {
  [0]=>
  int(1)
  [1]=>
  NULL
}
Undefined variable $undef
string(3) "str"
Undefined variable $undef
Undefined variable $undef
Undefined variable $undef
ok
Undefined variable $undef
int(1)
NULL
