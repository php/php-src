--TEST--
PDO OCI specific class constants
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_oci')) die('skip not loaded');
require(__DIR__.'/../../pdo/tests/pdo_test.inc');
PDOTest::skip();
?>
--FILE--
<?php

require(__DIR__ . '/../../pdo/tests/pdo_test.inc');

$expected = [
	'OCI_ATTR_CLIENT_INFO'        => true,
	'OCI_ATTR_ACTION'             => true,
	'OCI_ATTR_CLIENT_IDENTIFIER'  => true,
	'OCI_ATTR_MODULE'             => true,
];

$ref = new ReflectionClass('PDO');
$constants = $ref->getConstants();
$values = [];

foreach ($constants as $name => $value) {
	if (substr($name, 0, 8) == 'OCI_ATTR') {
		if (!isset($values[$value])) {
			$values[$value] = [$name];
        } else {
			$values[$value][] = $name;
        }

		if (isset($expected[$name])) {
			unset($expected[$name]);
			unset($constants[$name]);
		}

		} else {
			unset($constants[$name]);
		}
}

if (!empty($constants)) {
	printf("[001] Dumping list of unexpected constants\n");
	var_dump($constants);
}

if (!empty($expected)) {
	printf("[002] Dumping list of missing constants\n");
	var_dump($expected);
}

if (!empty($values)) {
	foreach ($values as $value => $constants) {
		if (count($constants) > 1) {
			printf("[003] Several constants share the same value '%s'\n", $value);
			var_dump($constants);
		}
	}
}

print "done!";
--EXPECT--
done!
