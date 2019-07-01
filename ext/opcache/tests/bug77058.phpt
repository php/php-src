--TEST--
Bug #77058: Type inference in opcache causes side effects
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php 

function myfunc(){
  $Nr = 0;
  while(1){
    $x--;
    $x++;
    if( ++ $Nr >= 2 ) break;
  }
  echo "'$Nr' is expected to be 2", PHP_EOL;
}
myfunc();

?>
--EXPECTF--
Notice: Undefined variable: x in %s on line %d
'2' is expected to be 2
