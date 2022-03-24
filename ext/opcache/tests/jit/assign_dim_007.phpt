--TEST--
JIT ASSIGN_DIM: 007
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
set_error_handler(function() {
	$GLOBALS['x'] = $GLOBALS['y'];
});

function x(&$s) {
	$s[0] = 1;
};
$y = false;
x($y);
var_dump($x,$y);
?>
--EXPECT--
array(0) {
}
array(1) {
  [0]=>
  int(1)
}
