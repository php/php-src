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
	echo $ex::class, ': ', $ex->getMessage(), "\n";
}
?>
--EXPECT--
Err: Creation of dynamic property class@anonymous::$y is deprecated
Error: Cannot create dynamic property class@anonymous::$y
