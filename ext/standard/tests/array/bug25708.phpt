--TEST--
Bug #25708 (extract($GLOBALS, EXTR_REFS) mangles $GLOBALS)
--FILE--
<?php
function foo($ref, $alt) {
    unset($GLOBALS['a']);
    unset($GLOBALS['b']);
    $GLOBALS['a'] = 1;
    $GLOBALS['b'] = 2;

    $org_a = $GLOBALS['a'];
    $org_b = $GLOBALS['b'];

    if ($ref) {
        global $a, $b;
    } else {
        /* zval temp_var(NULL); // refcount = 1
         * a = temp_var[x] // refcount = 2
         */
        $a = NULL;
        $b = NULL;
    }

    var_dump($a, $b, $GLOBALS['a'], $GLOBALS['b']);
    echo "--\n";
    if ($alt) {
        $a = &$GLOBALS['a'];
        $b = &$GLOBALS['b'];
    } else {
        extract($GLOBALS, EXTR_REFS);
    }
    var_dump($a, $b, $GLOBALS['a'], $GLOBALS['b']);
    echo "--\n";
    $a = &$GLOBALS['a'];
    $b = &$GLOBALS['b'];
    var_dump($a, $b, $GLOBALS['a'], $GLOBALS['b']);
    echo "--\n";
    $GLOBALS['b'] = 3;
    var_dump($a, $b, $GLOBALS['a'], $GLOBALS['b']);
    echo "--\n";
    $a = 4;
    var_dump($a, $b, $GLOBALS['a'], $GLOBALS['b']);
    echo "--\n";
    $c = $b;
    var_dump($b, $GLOBALS['b'], $c);
    echo "--\n";
    $b = 'x';
    var_dump($a, $b, $GLOBALS['a'], $GLOBALS['b'], $c);
    echo "--\n";
    var_dump($org_a, $org_b);
    echo "----";
    if ($ref) echo 'r';
    if ($alt) echo 'a';
    echo "\n";
}

$a = 'ok';
$b = 'ok';
$_a = $a;
$_b = $b;

foo(false, true);
foo(true, true);
foo(false, false);
foo(true, false);

var_dump($_a, $_b);
?>
--EXPECT--
NULL
NULL
int(1)
int(2)
--
int(1)
int(2)
int(1)
int(2)
--
int(1)
int(2)
int(1)
int(2)
--
int(1)
int(3)
int(1)
int(3)
--
int(4)
int(3)
int(4)
int(3)
--
int(3)
int(3)
int(3)
--
int(4)
string(1) "x"
int(4)
string(1) "x"
int(3)
--
int(1)
int(2)
----a
int(1)
int(2)
int(1)
int(2)
--
int(1)
int(2)
int(1)
int(2)
--
int(1)
int(2)
int(1)
int(2)
--
int(1)
int(3)
int(1)
int(3)
--
int(4)
int(3)
int(4)
int(3)
--
int(3)
int(3)
int(3)
--
int(4)
string(1) "x"
int(4)
string(1) "x"
int(3)
--
int(1)
int(2)
----ra
NULL
NULL
int(1)
int(2)
--
int(1)
int(2)
int(1)
int(2)
--
int(1)
int(2)
int(1)
int(2)
--
int(1)
int(3)
int(1)
int(3)
--
int(4)
int(3)
int(4)
int(3)
--
int(3)
int(3)
int(3)
--
int(4)
string(1) "x"
int(4)
string(1) "x"
int(3)
--
int(1)
int(2)
----
int(1)
int(2)
int(1)
int(2)
--
int(1)
int(2)
int(1)
int(2)
--
int(1)
int(2)
int(1)
int(2)
--
int(1)
int(3)
int(1)
int(3)
--
int(4)
int(3)
int(4)
int(3)
--
int(3)
int(3)
int(3)
--
int(4)
string(1) "x"
int(4)
string(1) "x"
int(3)
--
int(1)
int(2)
----r
string(2) "ok"
string(2) "ok"
