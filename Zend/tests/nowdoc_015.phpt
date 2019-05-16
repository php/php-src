--TEST--
Test nowdoc and line numbering
--FILE--
<?php
function error_handler($num, $msg, $file, $line) {
	echo $line,"\n";
}
set_error_handler('error_handler');
trigger_error("line", E_USER_ERROR);
$x = <<<EOF
EOF;
var_dump($x);
trigger_error("line", E_USER_ERROR);
$x = <<<'EOF'
EOF;
var_dump($x);
trigger_error("line", E_USER_ERROR);
$x = <<<EOF
test
EOF;
var_dump($x);
trigger_error("line", E_USER_ERROR);
$x = <<<'EOF'
test
EOF;
var_dump($x);
trigger_error("line", E_USER_ERROR);
$x = <<<EOF
test1
test2

test3


EOF;
var_dump($x);
trigger_error("line", E_USER_ERROR);
$x = <<<'EOF'
test1
test2

test3


EOF;
var_dump($x);
trigger_error("line", E_USER_ERROR);
echo "ok\n";
?>
--EXPECT--
6
string(0) ""
10
string(0) ""
14
string(4) "test"
19
string(4) "test"
24
string(20) "test1
test2

test3

"
34
string(20) "test1
test2

test3

"
44
ok
