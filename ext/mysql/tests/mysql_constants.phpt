--TEST--
Constants exported by ext/mysql
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
require("connect.inc");
require("table.inc");

$constants = get_defined_constants(true);
sort($constants);

$expected_constants = array(
	'MYSQL_ASSOC'                   => true,
	'MYSQL_NUM'                     => true,
	'MYSQL_BOTH'                    => true,
	'MYSQL_CLIENT_COMPRESS'         => true,
	'MYSQL_CLIENT_INTERACTIVE'      => true,
	'MYSQL_CLIENT_IGNORE_SPACE'     => true,
);

$version = mysql_get_server_info($link);
if (!preg_match('@(\d+)\.(\d+)\.(\d+)@ism', $version, $matches))
	printf("[001] Cannot get server version\n");
$version = ($matches[1] * 100) + ($matches[2] * 10) + $matches[3];

if ($version > 400) {
	$expected_constants = array_merge($expected_constants, array(
					"MYSQL_CLIENT_SSL"                => true,
					));
}


$unexpected_constants = array();

foreach ($constants as $group => $consts) {
	foreach ($consts as $name => $value) {
		if (stristr($name, 'mysql') && !preg_match("/^mysql([^_]+)_/iu", $name)) {
			$name = strtoupper($name);
			if (isset($expected_constants[$name])) {
				unset($expected_constants[$name]);
			} else {
				$unexpected_constants[$name] = $name;
			}
		}
	}
}

if (!empty($unexpected_constants)) {
	printf("Dumping list of unexpected constants\n");
	var_dump($unexpected_constants);
}

if (!empty($expected_constants)) {
	printf("Dumping list of missing constants\n");
	var_dump($expected_constants);
}

print "done!";
?>
--EXPECTF--
done!