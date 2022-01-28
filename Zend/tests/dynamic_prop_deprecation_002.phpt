--TEST--
Dynamic properties deprecation 002 (memory leak)
--FILE--
<?php
set_error_handler(function($code, $msg){
	echo "Err: $msg\n";
    $GLOBALS['a']=null;
});
$a = new class{};
[&$a->y];
?>
--EXPECT--
Err: Creation of dynamic property class@anonymous::$y is deprecated