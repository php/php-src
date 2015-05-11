--TEST--
Array access on non-array
--FILE--
<?php
$values = array(
    'null' => null,
    'int' => 42,
    'float' => 3.14159,
    'bool' => true,
    'string' => 'hello world',
    'object' => new StdClass(),
);

foreach ($values as $type => $a) {
    print "\n\n--- {$type} ---\n";
    var_dump($a[0]);
    var_dump($a[0][1]);
    var_dump($a['foo']);
    var_dump($a['foo']['bar']);
    $b = isset($a[0]);
    var_dump($b);
    $b = isset($a[0][0]);
    var_dump($b);
    unset($a, $b);
}
--EXPECTF--
--- null ---

Notice: Trying to get index of a non-array in /home/allen/src/php-src/Zend/tests/array_access_nonarray.php on line 13
NULL

Notice: Trying to get index of a non-array in /home/allen/src/php-src/Zend/tests/array_access_nonarray.php on line 14

Notice: Trying to get index of a non-array in /home/allen/src/php-src/Zend/tests/array_access_nonarray.php on line 14
NULL

Notice: Trying to get index of a non-array in /home/allen/src/php-src/Zend/tests/array_access_nonarray.php on line 15
NULL

Notice: Trying to get index of a non-array in /home/allen/src/php-src/Zend/tests/array_access_nonarray.php on line 16

Notice: Trying to get index of a non-array in /home/allen/src/php-src/Zend/tests/array_access_nonarray.php on line 16
NULL
bool(false)
bool(false)


--- int ---

Notice: Trying to get index of a non-array in /home/allen/src/php-src/Zend/tests/array_access_nonarray.php on line 13
NULL

Notice: Trying to get index of a non-array in /home/allen/src/php-src/Zend/tests/array_access_nonarray.php on line 14

Notice: Trying to get index of a non-array in /home/allen/src/php-src/Zend/tests/array_access_nonarray.php on line 14
NULL

Notice: Trying to get index of a non-array in /home/allen/src/php-src/Zend/tests/array_access_nonarray.php on line 15
NULL

Notice: Trying to get index of a non-array in /home/allen/src/php-src/Zend/tests/array_access_nonarray.php on line 16

Notice: Trying to get index of a non-array in /home/allen/src/php-src/Zend/tests/array_access_nonarray.php on line 16
NULL
bool(false)
bool(false)


--- float ---

Notice: Trying to get index of a non-array in /home/allen/src/php-src/Zend/tests/array_access_nonarray.php on line 13
NULL

Notice: Trying to get index of a non-array in /home/allen/src/php-src/Zend/tests/array_access_nonarray.php on line 14

Notice: Trying to get index of a non-array in /home/allen/src/php-src/Zend/tests/array_access_nonarray.php on line 14
NULL

Notice: Trying to get index of a non-array in /home/allen/src/php-src/Zend/tests/array_access_nonarray.php on line 15
NULL

Notice: Trying to get index of a non-array in /home/allen/src/php-src/Zend/tests/array_access_nonarray.php on line 16

Notice: Trying to get index of a non-array in /home/allen/src/php-src/Zend/tests/array_access_nonarray.php on line 16
NULL
bool(false)
bool(false)


--- bool ---

Notice: Trying to get index of a non-array in /home/allen/src/php-src/Zend/tests/array_access_nonarray.php on line 13
NULL

Notice: Trying to get index of a non-array in /home/allen/src/php-src/Zend/tests/array_access_nonarray.php on line 14

Notice: Trying to get index of a non-array in /home/allen/src/php-src/Zend/tests/array_access_nonarray.php on line 14
NULL

Notice: Trying to get index of a non-array in /home/allen/src/php-src/Zend/tests/array_access_nonarray.php on line 15
NULL

Notice: Trying to get index of a non-array in /home/allen/src/php-src/Zend/tests/array_access_nonarray.php on line 16

Notice: Trying to get index of a non-array in /home/allen/src/php-src/Zend/tests/array_access_nonarray.php on line 16
NULL
bool(false)
bool(false)


--- string ---
string(1) "h"

Notice: Uninitialized string offset: 1 in /home/allen/src/php-src/Zend/tests/array_access_nonarray.php on line 14
string(0) ""

Warning: Illegal string offset 'foo' in /home/allen/src/php-src/Zend/tests/array_access_nonarray.php on line 15
string(1) "h"

Warning: Illegal string offset 'foo' in /home/allen/src/php-src/Zend/tests/array_access_nonarray.php on line 16

Warning: Illegal string offset 'bar' in /home/allen/src/php-src/Zend/tests/array_access_nonarray.php on line 16
string(1) "h"
bool(true)
bool(true)


--- object ---

Fatal error: Cannot use object of type stdClass as array in /home/allen/src/php-src/Zend/tests/array_access_nonarray.php on line 13
