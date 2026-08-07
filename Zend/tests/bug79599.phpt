--TEST--
Bug #79599 (coredump in set_error_handler)
--FILE--
<?php
set_error_handler(function($code, $message){
    throw new \Exception($message);
});
function test1(){
    $a[] = $b;
}
function test2(){
    $a[$c] = $b;
}
try{
    test1();
}catch(\Exception $e){
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try{
    test2();
}catch(\Exception $e){
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Exception: Undefined variable $b
Exception: Undefined variable $c
