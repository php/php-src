--TEST--
PDO: bindParam() must not leak driver_params
--EXTENSIONS--
pdo
pdo_sqlite
--FILE--
<?php
class C {}
$db = new PDO('sqlite::memory:');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$stmt = $db->prepare('SELECT ?');

$n = 20000;
$dp = str_repeat('a', 1024);
$obj = new C();
try {
    $stmt->bindParam(1, $obj, PDO::PARAM_STR, 0, $dp);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
for ($i = 0; $i < $n; $i++) {
    $dp = str_repeat('a', 1024);
    $obj = new C();
    try {
        $stmt->bindParam(1, $obj, PDO::PARAM_STR, 0, $dp);
    } catch (Error $e) {
    }
}
$before = memory_get_usage();
for ($i = 0; $i < $n; $i++) {
    $dp = str_repeat('b', 1024);
    $obj = new C();
    try {
        $stmt->bindParam(1, $obj, PDO::PARAM_STR, 0, $dp);
    } catch (Error $e) {
    }
}
$diff = memory_get_usage() - $before;
if ($diff > 1000) {
    echo "LEAK\n";
} else {
    echo "OK\n";
}

$stmt2 = $db->prepare('SELECT :bar');
$v = 'x';
for ($i = 0; $i < $n; $i++) {
    $dp = str_repeat('c', 1024);
    try {
        $stmt2->bindParam(':missing', $v, PDO::PARAM_STR, 0, $dp);
    } catch (PDOException $e) {
    }
}
$before = memory_get_usage();
for ($i = 0; $i < $n; $i++) {
    $dp = str_repeat('d', 1024);
    try {
        $stmt2->bindParam(':missing', $v, PDO::PARAM_STR, 0, $dp);
    } catch (PDOException $e) {
    }
}
$diff = memory_get_usage() - $before;
if ($diff > 1000) {
    echo "LEAK\n";
} else {
    echo "OK\n";
}

for ($i = 0; $i < $n; $i++) {
    $dp = str_repeat('e', 1024);
    $stmt->bindParam(1, $v, PDO::PARAM_STR, 0, $dp);
}
$before = memory_get_usage();
for ($i = 0; $i < $n; $i++) {
    $dp = str_repeat('f', 1024);
    $stmt->bindParam(1, $v, PDO::PARAM_STR, 0, $dp);
}
$diff = memory_get_usage() - $before;
if ($diff > 1000) {
    echo "LEAK\n";
} else {
    echo "OK\n";
}
?>
--EXPECT--
Error: Object of class C could not be converted to string
OK
OK
OK
