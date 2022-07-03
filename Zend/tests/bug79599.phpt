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
    var_dump($e->getMessage());
}
try{
    test2();
}catch(\Exception $e){
    var_dump($e->getMessage());
}
?>
--EXPECT--
string(60) "Undefined variable $b (This will become an error in PHP 9.0)"
string(60) "Undefined variable $c (This will become an error in PHP 9.0)"
