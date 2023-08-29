--TEST--
Bug #78598: Changing array during undef index RW error segfaults
--INI--
opcache.jit=0
--FILE--
<?php

$my_var = null;
set_error_handler(function() use(&$my_var) {
    $my_var = 0;
});

$my_var[0] .= "xyz";
var_dump($my_var);

$my_var = null;
$my_var[0][0][0] .= "xyz";
var_dump($my_var);

$my_var = null;
$my_var["foo"] .= "xyz";
var_dump($my_var);

$my_var = null;
$my_var["foo"]["bar"]["baz"] .= "xyz";
var_dump($my_var);

?>
--EXPECT--
int(0)
int(0)
int(0)
int(0)
