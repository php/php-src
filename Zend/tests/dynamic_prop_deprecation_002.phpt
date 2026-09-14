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
--EXPECTF--
Err: Creation of dynamic property class@anonymous%0%s::$y is deprecated
Error: Cannot create dynamic property class@anonymous%0%s::$y
