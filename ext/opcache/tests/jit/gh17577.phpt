--TEST--
GH-17577 (JIT packed type guard crash)
--EXTENSIONS--
opcache
--INI--
opcache.jit_buffer_size=16M
opcache.jit_hot_func=1
--FILE--
<?php
$a = array(
    array(1,2,3),
    0,
);
function my_dump($var) {
}
foreach($a as $b) {
    for ($i = 0; $i < 3; $i++) {
        my_dump($b[$i]);
    }
}
?>
--EXPECTF--
Warning: Trying to access array offset on int in %s on line %d

Warning: Trying to access array offset on int in %s on line %d

Warning: Trying to access array offset on int in %s on line %d
