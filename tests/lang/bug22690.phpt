--TEST--
Bug #22690 (ob_start() is broken with create_function() callbacks)
--FILE--
<?php
	$foo = create_function('$s', 'return strtoupper($s);');
	ob_start($foo);
	echo $foo("bar\n");
?>
bar
--EXPECT--
BAR
BAR
