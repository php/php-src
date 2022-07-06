--TEST--
file_put_contents() test
--FILE--
<?php
    chdir(__DIR__);
    for ($i = 1; $i < 6; $i++) {
        @unlink("./TEST{$i}");
    }

    echo "String Test: ";
    echo file_put_contents("TEST1", file_get_contents(__FILE__)) !== FALSE ? 'OK' : 'FAIL';
    echo "\n";

    $old_int = $int = rand();
    $ret = file_put_contents("TEST2", $int);
    echo "Integer Test: ";
    if ($int === $old_int && $ret !== FALSE && md5($int) == md5_file("TEST2")) {
        echo 'OK';
    } else {
        echo 'FAIL';
    }
    echo "\n";

    $old_int = $int = time() / 1000;
    $ret = file_put_contents("TEST3", $int);
    echo "Float Test: ";
    if ($int === $old_int && $ret !== FALSE && md5($int) == md5_file("TEST3")) {
        echo 'OK';
    } else {
        echo 'FAIL';
    }
    echo "\n";

    $ret = file_put_contents("TEST4", __FILE__);
    echo "Bool Test: ";
    if ($ret !== FALSE && md5(__FILE__) == md5_file("TEST4")) {
        echo 'OK';
    } else {
        echo 'FAIL';
    }
    echo "\n";

    $ret = @file_put_contents("TEST5", $_SERVER);
    echo "Array Test: ";
    if ($ret !== FALSE && @md5(implode('', $_SERVER)) == md5_file("TEST5")) {
        echo 'OK';
    } else {
        echo 'FAIL';
    }
    echo "\n";

    for ($i = 1; $i < 6; $i++) {
        @unlink("./TEST{$i}");
    }
?>
--EXPECT--
String Test: OK
Integer Test: OK
Float Test: OK
Bool Test: OK
Array Test: OK
