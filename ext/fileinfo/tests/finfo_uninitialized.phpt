--TEST--
Fileinfo uninitialized object
--EXTENSIONS--
fileinfo
--FILE--
<?php

$finfo = (new ReflectionClass('finfo'))->newInstanceWithoutConstructor();

try {
	var_dump(finfo_set_flags($finfo, FILEINFO_NONE));
} catch (Error $e) {
	echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

try {
	var_dump($finfo->set_flags(FILEINFO_NONE));
} catch (Error $e) {
	echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

try {
	var_dump(finfo_file($finfo, __FILE__));
} catch (Error $e) {
	echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

try {
	var_dump($finfo->file(__FILE__));
} catch (Error $e) {
	echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

try {
	var_dump(finfo_buffer($finfo, file_get_contents(__FILE__)));
} catch (Error $e) {
	echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

try {
	var_dump($finfo->file(file_get_contents(__FILE__)));
} catch (Error $e) {
	echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Error: Invalid finfo object
Error: Invalid finfo object
Error: Invalid finfo object
Error: Invalid finfo object
Error: Invalid finfo object
Error: Invalid finfo object
