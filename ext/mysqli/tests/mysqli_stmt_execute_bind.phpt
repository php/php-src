--TEST--
mysqli_stmt_execute() - bind in execute
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
    die(sprintf('skip Cannot connect to MySQL, [%d] %s.', mysqli_connect_errno(), mysqli_connect_error()));
}
if (mysqli_get_server_version($link) <= 40100) {
    die(sprintf('skip Needs MySQL 4.1+, found version %d.', mysqli_get_server_version($link)));
}
?>
--FILE--
<?php
    require_once("connect.inc");

    require('table.inc');

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
    } catch (mysqli_sql_exception $e) {
        echo '[001] '.$e->getMessage()."\n";
    }
    $stmt = null;

    // 3. param number has to match - missing all parameters
    $stmt = $link->prepare('SELECT label, ? AS anon, ? AS num FROM test WHERE id=?');
    try {
        $stmt->execute([]);
    } catch (mysqli_sql_exception $e) {
        echo '[002] '.$e->getMessage()."\n";
    }
    $stmt = null;

    // 4. param number has to match - missing argument to execute()
    $stmt = $link->prepare('SELECT label, ? AS anon, ? AS num FROM test WHERE id=?');
    try {
        $stmt->execute();
    } catch (mysqli_sql_exception $e) {
        echo '[003] '.$e->getMessage()."\n";
    }
    $stmt = null;

    // 5. wrong argument to execute()
    $stmt = $link->prepare('SELECT label, ? AS anon, ? AS num FROM test WHERE id=?');
    try {
        $stmt->execute(42);
    } catch (TypeError $e) {
		echo '[004] '.$e->getMessage()."\n";
    }
    $stmt = null;

    // 6. objects are not arrays and are not accepted
    $stmt = $link->prepare('SELECT label, ? AS anon, ? AS num FROM test WHERE id=?');
    try {
        $stmt->execute((object)[&$abc, 42, $id]);
    } catch (TypeError $e) {
        echo '[005] '.$e->getMessage()."\n";
    }
    $stmt = null;

    // 7. arrays by reference work too
    $stmt = $link->prepare('SELECT label, ? AS anon, ? AS num FROM test WHERE id=?');
    $arr = [&$abc, 42, $id];
    $arr2 = &$arr;
    $stmt->execute($arr2);
    assert($stmt->get_result()->fetch_assoc() === ['label'=>'a', 'anon'=>'abc', 'num' => '42']);
    $stmt = null;

    // 8. no placeholders in statement. params are ignored
    $stmt = $link->prepare('SELECT label FROM test WHERE id=1');
    $stmt->execute(['I am ignored']);
    assert($stmt->get_result()->fetch_assoc() === ['label'=>'a']);
    $stmt = null;

    // 9. once bound the values are persisted. Just like in PDO
    $stmt = $link->prepare('SELECT label, ? AS anon, ? AS num FROM test WHERE id=?');
    $stmt->execute(['abc', 42, $id]);
    assert($stmt->get_result()->fetch_assoc() === ['label'=>'a', 'anon'=>'abc', 'num' => '42']);
    $stmt->execute(); // no argument here. Values are already bound
    assert($stmt->get_result()->fetch_assoc() === ['label'=>'a', 'anon'=>'abc', 'num' => '42']);
    try {
		$stmt->execute([]); // no params here. PDO doesn't throw an error, but mysqli does
    } catch (mysqli_sql_exception $e) {
        echo '[006] '.$e->getMessage()."\n";
    }
    $stmt = null;

    // 10. mixing binding styles not possible. Also, NULL should stay NULL when bound as string
    $stmt = $link->prepare('SELECT label, ? AS anon, ? AS num FROM test WHERE id=?');
    $stmt->bind_param('sss', ...['abc', 42, null]);
	$stmt->execute([null, null, $id]);
    assert($stmt->get_result()->fetch_assoc() === ['label'=>'a', 'anon'=>null, 'num' => null]);
    $stmt = null;

    // 11. array keys are ignored. Even numerical indices are not considered (PDO does a weird thing with the numerical indices)
    $stmt = $link->prepare('SELECT label, ? AS anon, ? AS num FROM test WHERE id=?');
	$stmt->execute(['A'=>'abc', 2=>42, null=>$id]);
    assert($stmt->get_result()->fetch_assoc() === ['label'=>'a', 'anon'=>'abc', 'num' => '42']);
    $stmt = null;


    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECT--
[001] No data supplied for 1 parameter in prepared statement
[002] No data supplied for 3 parameters in prepared statement
[003] No data supplied for parameters in prepared statement
[004] mysqli_stmt::execute(): Argument #1 ($params) must be of type array, int given
[005] mysqli_stmt::execute(): Argument #1 ($params) must be of type array, stdClass given
[006] No data supplied for 3 parameters in prepared statement
done!
