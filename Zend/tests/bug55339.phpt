--TEST--
Bug #55339 (Segfault with allow_call_time_pass_reference = Off)
--INI--
allow_call_time_pass_reference=off
--FILE--
<?php
function error_handler($errno, $errstr, $errfile, $errline) {
	eval(';');
}

set_error_handler('error_handler');

eval(<<<'EOF'
function foo()
{
    $array = array();
    foreach ($array as $key => $value) {
        bar($key, &$value);
    }
}

function bar()
{

}
EOF
);

echo "OK\n";
--EXPECT--
OK
