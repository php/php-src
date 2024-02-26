--TEST--
PDO Common: PDORow + get_parent_class()
--EXTENSIONS--
pdo
--SKIPIF--
<?php
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();

const TABLE_NAME = 'test_pdo_35_pdo_row';
$db->exec('CREATE TABLE ' . TABLE_NAME .' (id int)');
$db->exec('INSERT INTO ' . TABLE_NAME .' VALUES (23)');

$stmt = $db->prepare('SELECT id FROM ' . TABLE_NAME);
$stmt->execute();
$result = $stmt->fetch(PDO::FETCH_LAZY);

var_dump($result);
var_dump(get_parent_class($result));

foreach ([0, "0", "id"] as $offset) {
    echo 'Offset: ', var_export($offset), PHP_EOL;
    $offsetRef = &$offset;

    echo 'Isset:', PHP_EOL;
    var_dump(isset($result[$offset]));
    var_dump(isset($result[$offsetRef]));
    echo 'Empty:', PHP_EOL;
    var_dump(empty($result[$offset]));
    var_dump(empty($result[$offsetRef]));
    echo 'Null coalesce:', PHP_EOL;
    var_dump($result[$offset] ?? "default");
    var_dump($result[$offsetRef] ?? "default");
    echo 'Read:', PHP_EOL;
    var_dump($result[$offset]);
    var_dump($result[$offsetRef]);
}

echo 'Errors:', PHP_EOL;
try {
    $result[0] = 1;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $result[] = 1;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $refResult = &$result[0];
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $refResult = &$result[];
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    unset($result[0]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $result->foo = 1;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    unset($result->foo);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
object(PDORow)#3 (2) {
  ["queryString"]=>
  string(34) "SELECT id FROM test_pdo_35_pdo_row"
  ["id"]=>
  string(2) "23"
}
bool(false)
Offset: 0
Isset:
bool(false)
bool(false)
Empty:
bool(false)
bool(false)
Null coalesce:
string(2) "23"
string(2) "23"
Read:
string(2) "23"
string(2) "23"
Offset: '0'
Isset:
bool(false)
bool(false)
Empty:
bool(false)
bool(false)
Null coalesce:
string(2) "23"
string(2) "23"
Read:
string(2) "23"
string(2) "23"
Offset: 'id'
Isset:
bool(true)
bool(true)
Empty:
bool(false)
bool(false)
Null coalesce:
string(2) "23"
string(2) "23"
Read:
string(2) "23"
string(2) "23"
Errors:
Cannot write to PDORow offset
Cannot write to PDORow offset

Notice: Indirect modification of overloaded element of PDORow has no effect in /home/girgias/Dev/PHP-8.2/ext/pdo_sqlite/tests/pdo_035.php on line 43
AddressSanitizer:DEADLYSIGNAL
=================================================================
==3997945==ERROR: AddressSanitizer: SEGV on unknown address 0x000000000008 (pc 0x000000dfad5b bp 0x7ffc2d5967e0 sp 0x7ffc2d5967d0 T0)
==3997945==The signal is caused by a READ memory access.
==3997945==Hint: address points to the zero page.
    #0 0xdfad5b in zval_get_type /home/girgias/Dev/PHP-8.2/Zend/zend_types.h:582
    #1 0xe10769 in row_dim_read /home/girgias/Dev/PHP-8.2/ext/pdo/pdo_stmt.c:2299
    #2 0x16f0c7e in zend_fetch_dimension_address /home/girgias/Dev/PHP-8.2/Zend/zend_execute.c:2534
    #3 0x16f156b in zend_fetch_dimension_address_W /home/girgias/Dev/PHP-8.2/Zend/zend_execute.c:2609
    #4 0x182c7a6 in ZEND_FETCH_DIM_W_SPEC_CV_UNUSED_HANDLER /home/girgias/Dev/PHP-8.2/Zend/zend_vm_execute.h:47361
    #5 0x1859bbc in execute_ex /home/girgias/Dev/PHP-8.2/Zend/zend_vm_execute.h:60042
    #6 0x185b4d4 in zend_execute /home/girgias/Dev/PHP-8.2/Zend/zend_vm_execute.h:60439
    #7 0x167029b in zend_execute_scripts /home/girgias/Dev/PHP-8.2/Zend/zend.c:1840
    #8 0x150533f in php_execute_script /home/girgias/Dev/PHP-8.2/main/main.c:2557
    #9 0x1a2b6cc in do_cli /home/girgias/Dev/PHP-8.2/sapi/cli/php_cli.c:964
    #10 0x1a2d2aa in main /home/girgias/Dev/PHP-8.2/sapi/cli/php_cli.c:1333
    #11 0x7fd7f6c46149 in __libc_start_call_main (/usr/lib64/../lib64/libc.so.6+0x28149) (BuildId: 7ea8d85df0e89b90c63ac7ed2b3578b2e7728756)
    #12 0x7fd7f6c4620a in __libc_start_main_impl (/usr/lib64/../lib64/libc.so.6+0x2820a) (BuildId: 7ea8d85df0e89b90c63ac7ed2b3578b2e7728756)
    #13 0x609b64 in _start (/home/girgias/Dev/PHP-8.2/sapi/cli/php+0x609b64) (BuildId: a4605c8d283f19dad7a72000fb964237c2b0f7a2)

AddressSanitizer can not provide additional info.
SUMMARY: AddressSanitizer: SEGV /home/girgias/Dev/PHP-8.2/Zend/zend_types.h:582 in zval_get_type
==3997945==ABORTING
