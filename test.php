<?php
function test() {
	echo "Hello World\n";
}
function test2() {
    echo "Hello World 2\n";
}

if (!isset($greeting)) {
    echo test();
}

phpdbg_break();

test2();
return true;
?>
