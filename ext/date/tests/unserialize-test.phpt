--TEST--
Test DateInterval::__unserialize OSS fuzz issues
--FILE--
<?php
$files = [
	'ossfuzz-55589.txt',
	'ossfuzz-55599.txt',
	'ossfuzz-55727.txt',
	'ossfuzz-56931.txt',
];

foreach ($files as $file) {
	echo "{$file}: ";

	$s = file_get_contents(__DIR__ . "/{$file}");

	try {
		$x = unserialize(substr($s, strpos($s, "|") + 1));
	} catch (Error $e) {
		echo get_class($e), ': ', $e->getMessage(), "\n";
	}
	var_dump($x);
	echo "\n\n";
}
?>
--EXPECTF--
ossfuzz-55589.txt: 
%s: unserialize(): Error at offset 39 of 39 bytes in %sunserialize-test.php on line %d
bool(false)


ossfuzz-55599.txt: 
%s: unserialize(): Error at offset 26 of 26 bytes in %sunserialize-test.php on line %d
Error: Invalid serialization data for DateTime object
bool(false)


ossfuzz-55727.txt: 
%s: unserialize(): Error at offset 230 of 509 bytes in %sunserialize-test.php on line %d
bool(false)


ossfuzz-56931.txt: 
%s: unserialize(): Error at offset 39 of 39 bytes in %sunserialize-test.php on line %d

Deprecated: Creation of dynamic property DateInterval::$ is deprecated in %s on line %d
bool(false)
