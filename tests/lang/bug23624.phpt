--TEST--
Bug #23624 (foreach leaves current array key as null)
--FILE--
<?php
    $arr = array ('one', 'two', 'three');
    var_dump(current($arr));
    foreach($arr as $key => $value);
    var_dump(current($arr));
?>
--EXPECT--
string(3) "one"
string(5) "three"
