--TEST--
jdtogregorian(): test overflow
--CREDITS--
neweracracker@gmail.com
--EXTENSIONS--
calendar
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die('skip this test is for 32bit platforms only');
?>
--FILE--
<?php
for ($i=536838860; $i<536838870; $i++) {
    echo $i, ':', jdtogregorian($i), PHP_EOL;
}
echo 'DONE', PHP_EOL;
?>
--EXPECT--
536838860:10/11/1465102
536838861:10/12/1465102
536838862:10/13/1465102
536838863:10/14/1465102
536838864:10/15/1465102
536838865:10/16/1465102
536838866:10/17/1465102
536838867:0/0/0
536838868:0/0/0
536838869:0/0/0
DONE
