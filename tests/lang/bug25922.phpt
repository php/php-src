--TEST--
Bug #25922 (SEGV in error_handler when context is destroyed)
--INI--
error_reporting=2047
--FILE--
<?php
function my_error_handler($error, $errmsg='', $errfile='', $errline=0, $errcontext='')
{
    echo "$errmsg\n";
    $errcontext = '';
}

set_error_handler('my_error_handler');

function test()
{
    echo "Undefined index here: '{$data['HTTP_HEADER']}'\n";
}
test();
?>
--EXPECT--
Undefined variable $data
Trying to access array offset on null
Undefined index here: ''
