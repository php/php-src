--TEST--
Bug #74431 - foreach infinite loop
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=0xffffffff
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function test(){
    $arr = [1,2];
    $j = 0;
    $cond = true;
    foreach ($arr as $i => $v){
        while(1){
            if($cond){
                break;
            }
        }
        $j++;
        echo $j."\n";
        if ($j>10) break;
    }
}
test();
?>
--EXPECT--
1
2
