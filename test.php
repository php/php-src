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

test2();
return true;
?>
