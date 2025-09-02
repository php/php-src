--TEST--
Dynamic properties deprecation 002 (memory leak)
--FILE--
<?php
set_error_handler(function($code, $msg){
	echo "Err: $msg\n";
    $GLOBALS['a']=null;
});
$a = new class{};
try {
    [&$a->y];
} catch (Throwable $ex) {
	echo "Exception: " .$ex->getMessage() . "\n";
}
?>
--EXPECT--
Err: Creation of dynamic property class@anonymous::$y is deprecated
Exception: Cannot create dynamic property class@anonymous::$y
