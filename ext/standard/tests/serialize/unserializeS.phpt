--TEST--
Bug MOPB-29 (wrong length calculation for S)
--INI--
error_reporting=0
--FILE--
<?php
$str = 'S:'.(100*3).':"'.str_repeat('\61', 100).'"';
$arr = array(str_repeat('"', 200)."1"=>1,str_repeat('"', 200)."2"=>1);

$data = unserialize($str);
var_dump($data);

--EXPECT--
bool(false)
