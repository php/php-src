--TEST--
ASSIGN_OBJ: error handler releases the fetched container while reporting undefined OP_DATA
--FILE--
<?php
set_error_handler(function ($_, $msg) {
    echo $msg, "\n";
    $GLOBALS['a'] = 0;
});

function dim() {
    global $a;
    $a = [new stdClass];
    $a[0]->p = $undef;
    var_dump($a);
}

function dim_op() {
    global $a;
    $o = new stdClass;
    $o->p = 'x';
    $a = [$o];
    unset($o);
    $a[0]->p .= $undef;
    var_dump($a);
}

function prop() {
    global $a;
    $a = new stdClass;
    $a->o = new stdClass;
    $a->o->p = $undef;
    var_dump($a);
}

dim();
dim_op();
prop();
?>
--EXPECT--
Undefined variable $undef
int(0)
Undefined variable $undef
int(0)
Undefined variable $undef
int(0)
