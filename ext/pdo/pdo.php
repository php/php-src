<?php
dl('pdo.so');
dl('pdo_sqlite.so');

//$x = new PDO("oci:dbname=hostname", 'php', 'php');
$x = new PDO("sqlite::memory:");

$x->query("create table test(name string, value string)");
debug_zval_dump($x);

$stmt = $x->prepare("INSERT INTO test (NAME, VALUE) VALUES (:name, :value)");

$stmt->bindParam(":name", $the_name, PDO_PARAM_STR, 32);
$stmt->bindParam(":value", $the_value, PDO_PARAM_STR, 32);

for ($i = 0; $i < 4; $i++) {
	$the_name = "foo" . rand();
	$the_value = "bar" . rand();

	if (!$stmt->execute()) {
		break;
	}
}

$stmt = null;

echo "DEFAULT:\n";
foreach ($x->query("select NAME, VALUE from test") as $row) {
	print_r($row);
}

echo "OBJ:\n";

class Foo {
	public $NAME = "Don't change me";
}

$foo = new foo;

foreach ($x->query("select NAME, VALUE from test", PDO_FETCH_COLUMN, 1) as $row) {
	debug_zval_dump($row);
}

echo "Done\n";
exit;

$stmt = $x->prepare("select NAME, VALUE from test where value like ?");
$the_name = 'bar%';
$stmt->execute(array($the_name)) or die("failed to execute!");
$stmt->bindColumn('VALUE', $value);

while ($row = $stmt->fetch()) {
	echo "name=$row[NAME] value=$row[VALUE]\n";
	echo "value is $value\n";
	echo "\n";
}

echo "Let's try an update\n";

echo "All done\n";

?>
