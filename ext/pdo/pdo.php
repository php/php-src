<?php

//$x = new PDO("oci:dbname=hostname", 'php', 'php');
$x = new PDO("odbc:ram", 'php', 'php', array(PDO_ATTR_AUTOCOMMIT => 0));
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

echo "All done\n";

?>
