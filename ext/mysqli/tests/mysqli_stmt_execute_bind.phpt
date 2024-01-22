--TEST--
mysqli_stmt_execute() - bind in execute
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
require 'table.inc';

mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);

// first, control case
$id = 1;
$abc = 'abc';
$stmt = $link->prepare('SELECT label, ? AS anon, ? AS num FROM test WHERE id=?');
$stmt->bind_param('sss', ...[&$abc, 42, $id]);
$stmt->execute();
assert($stmt->get_result()->fetch_assoc() === ['label'=>'a', 'anon'=>'abc', 'num' => '42']);
$stmt = null;

// 1. same as the control case, but skipping the middle-man (bind_param)
$stmt = $link->prepare('SELECT label, ? AS anon, ? AS num FROM test WHERE id=?');
$stmt->execute([&$abc, 42, $id]);
assert($stmt->get_result()->fetch_assoc() === ['label'=>'a', 'anon'=>'abc', 'num' => '42']);
$stmt = null;

// 2. param number has to match - missing 1 parameter
$stmt = $link->prepare('SELECT label, ? AS anon, ? AS num FROM test WHERE id=?');
try {
    $stmt->execute([&$abc, 42]);
} catch (ValueError $e) {
    echo '[001] '.$e->getMessage()."\n";
}
$stmt = null;

// 3. Too many parameters 
$stmt = $link->prepare('SELECT label, ? AS anon, ? AS num FROM test WHERE id=?');
try {
    $stmt->execute([&$abc, null, $id, 24]);
} catch (ValueError $e) {
    echo '[002] '.$e->getMessage()."\n";
}
$stmt = null;

// 4. param number has to match - missing all parameters
$stmt = $link->prepare('SELECT label, ? AS anon, ? AS num FROM test WHERE id=?');
try {
    $stmt->execute([]);
} catch (ValueError $e) {
    echo '[003] '.$e->getMessage()."\n";
}
$stmt = null;

// 5. param number has to match - missing argument to execute()
$stmt = $link->prepare('SELECT label, ? AS anon, ? AS num FROM test WHERE id=?');
try {
    $stmt->execute();
} catch (mysqli_sql_exception $e) {
    echo '[004] '.$e->getMessage()."\n";
}
$stmt = null;

// 6. wrong argument to execute()
$stmt = $link->prepare('SELECT label, ? AS anon, ? AS num FROM test WHERE id=?');
try {
    $stmt->execute(42);
} catch (TypeError $e) {
    echo '[005] '.$e->getMessage()."\n";
}
$stmt = null;

// 7. objects are not arrays and are not accepted
$stmt = $link->prepare('SELECT label, ? AS anon, ? AS num FROM test WHERE id=?');
try {
    $stmt->execute((object)[&$abc, 42, $id]);
} catch (TypeError $e) {
    echo '[006] '.$e->getMessage()."\n";
}
$stmt = null;

// 8. arrays by reference work too
$stmt = $link->prepare('SELECT label, ? AS anon, ? AS num FROM test WHERE id=?');
$arr = [&$abc, 42, $id];
$arr2 = &$arr;
$stmt->execute($arr2);
assert($stmt->get_result()->fetch_assoc() === ['label'=>'a', 'anon'=>'abc', 'num' => '42']);
$stmt = null;

// 9. no placeholders in statement. nothing to bind in an empty array
$stmt = $link->prepare('SELECT label FROM test WHERE id=1');
$stmt->execute([]);
assert($stmt->get_result()->fetch_assoc() === ['label'=>'a']);
$stmt = null;

// 10. once bound the values are persisted. Just like in PDO
$stmt = $link->prepare('SELECT label, ? AS anon, ? AS num FROM test WHERE id=?');
$stmt->execute(['abc', 42, $id]);
assert($stmt->get_result()->fetch_assoc() === ['label'=>'a', 'anon'=>'abc', 'num' => '42']);
$stmt->execute(); // no argument here. Values are already bound
assert($stmt->get_result()->fetch_assoc() === ['label'=>'a', 'anon'=>'abc', 'num' => '42']);
try {
    $stmt->execute([]); // no params here. PDO doesn't throw an error, but mysqli does
} catch (ValueError $e) {
    echo '[007] '.$e->getMessage()."\n";
}
$stmt = null;

// 11. mixing binding styles not possible. Also, NULL should stay NULL when bound as string
$stmt = $link->prepare('SELECT label, ? AS anon, ? AS num FROM test WHERE id=?');
$stmt->bind_param('sss', ...['abc', 42, null]);
$stmt->execute([null, null, $id]);
assert($stmt->get_result()->fetch_assoc() === ['label'=>'a', 'anon'=>null, 'num' => null]);
$stmt = null;

// 12. Only list arrays are allowed
$stmt = $link->prepare('SELECT label, ? AS anon, ? AS num FROM test WHERE id=?');
try {
    $stmt->execute(['A'=>'abc', 2=>42, null=>$id]);
} catch (ValueError $e) {
    echo '[008] '.$e->getMessage()."\n";
}
$stmt = null;


mysqli_close($link);
?>
--CLEAN--
<?php
require_once 'clean_table.inc';
?>
--EXPECT--
[001] mysqli_stmt::execute(): Argument #1 ($params) must consist of exactly 3 elements, 2 present
[002] mysqli_stmt::execute(): Argument #1 ($params) must consist of exactly 3 elements, 4 present
[003] mysqli_stmt::execute(): Argument #1 ($params) must consist of exactly 3 elements, 0 present
[004] No data supplied for parameters in prepared statement
[005] mysqli_stmt::execute(): Argument #1 ($params) must be of type ?array, int given
[006] mysqli_stmt::execute(): Argument #1 ($params) must be of type ?array, stdClass given
[007] mysqli_stmt::execute(): Argument #1 ($params) must consist of exactly 3 elements, 0 present
[008] mysqli_stmt::execute(): Argument #1 ($params) must be a list array
