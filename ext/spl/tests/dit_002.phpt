--TEST--
SPL: DirectoryIterator defaults
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php

$classes = array(
	'DirectoryIterator' => 0,
	'RecursiveDirectoryIterator' => 1,
);

foreach ($classes as $class => $flags) {
	echo "===$class===\n";
	$ref = new ReflectionClass($class);
	$obj = $ref->newInstance('.');
	echo get_class($obj->current()) . "\n";
	if ($flags)
	{
		$flags = array(
			RecursiveDirectoryIterator::CURRENT_AS_FILEINFO => 0,
			RecursiveDirectoryIterator::CURRENT_AS_SELF     => 0,
			RecursiveDirectoryIterator::CURRENT_AS_PATHNAME => 1,
		);
		foreach($flags as $flag => $isstring) {
			$obj = $ref->newInstance('.', $flag);
			if ($isstring) {
				$val = $obj->current();
				if (is_string($val)) {
					var_dump(true);
				} else {
					var_dump($val);
				}
			} else {
				echo get_class($obj->current()) . "\n";
			}
		}
	}
}
?>
===DONE===
--EXPECT--
===DirectoryIterator===
DirectoryIterator
===RecursiveDirectoryIterator===
SplFileInfo
SplFileInfo
RecursiveDirectoryIterator
bool(true)
===DONE===
