--TEST--
PEAR::phpVersionIs()
--SKIPIF--
--FILE--
<?php
// Test for PEAR::phpVersionIs()

require_once '../PEAR.php';

define (PHP_CURRENT_VER, '4.0.7');

function cmp($param1=null, $param2=null, $expected)
{
    echo PHP_CURRENT_VER;
    if ($param1) {
        echo " minor than $param1";
    }
    if ($param2) {
        echo " greater than $param2";
    }
    $res = PEAR::phpVersionIs($param1, $param2, PHP_CURRENT_VER);
    echo " is ";
    echo ($res) ? "TRUE" : "FALSE";
    if ($expected === $res) {
        echo "     OK\n";
    } else {
        echo "     FAIL\n";
    }
}
$vers = array(
    array('4.0.5', null, false),
    array('4.0.99', null, true),
    array(PHP_CURRENT_VER, null, false),
    array('4.0.99pl1', null, true),
    array('4.0.99RC1', null, true),
    array('4.0.99-dev', null, true),
    array(null, '4.0.5', true),
    array(null, '4.0.99', false),
    array(null, PHP_CURRENT_VER, false),
    array('4.0.5', '4.0.99', false),
    array('4.0.99', '4.0.5', true),
    array(PHP_CURRENT_VER, '4.0.5', false),
    array('4.0.99', PHP_CURRENT_VER, false)
);

foreach ($vers as $ver) {
    cmp($ver[0], $ver[1], $ver[2]);
}
?>
--GET--
--POST--
--EXPECT--
4.0.7 minor than 4.0.5 is FALSE     OK
4.0.7 minor than 4.0.99 is TRUE     OK
4.0.7 minor than 4.0.7 is FALSE     OK
4.0.7 minor than 4.0.99pl1 is TRUE     OK
4.0.7 minor than 4.0.99RC1 is TRUE     OK
4.0.7 minor than 4.0.99-dev is TRUE     OK
4.0.7 greater than 4.0.5 is TRUE     OK
4.0.7 greater than 4.0.99 is FALSE     OK
4.0.7 greater than 4.0.7 is FALSE     OK
4.0.7 minor than 4.0.5 greater than 4.0.99 is FALSE     OK
4.0.7 minor than 4.0.99 greater than 4.0.5 is TRUE     OK
4.0.7 minor than 4.0.7 greater than 4.0.5 is FALSE     OK
4.0.7 minor than 4.0.99 greater than 4.0.7 is FALSE     OK
