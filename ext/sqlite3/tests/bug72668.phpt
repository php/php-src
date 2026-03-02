--TEST--
Bug #72668 (Spurious warning when exception is thrown in user defined function)
--EXTENSIONS--
sqlite3
--FILE--
<?php
function my_udf_md5($string) {
    throw new \Exception("test exception\n");
}

$db = new SQLite3(':memory:');
$db->createFunction('my_udf_md5', 'my_udf_md5');

try {
    $result = $db->query('SELECT my_udf_md5("test")');
    var_dump($result);
}
catch(\Exception $e) {
    echo "Exception: ".$e->getMessage();
}
try {
    $result = $db->querySingle('SELECT my_udf_md5("test")');
    var_dump($result);
}
catch(\Exception $e) {
    echo "Exception: ".$e->getMessage();
}
$statement = $db->prepare('SELECT my_udf_md5("test")');
try {
    $result = $statement->execute();
    var_dump($result);
}
catch(\Exception $e) {
    echo "Exception: ".$e->getMessage();
}
?>
--EXPECT--
Exception: test exception
Exception: test exception
Exception: test exception
