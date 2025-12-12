--TEST--
sscanf(): test %[] specifier with edge cases
--FILE--
<?php

try {
	sscanf('[hello]', '%[][helo]', $out);
    var_dump($out);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
	sscanf('-in-', '%[-i-n]', $out);
    var_dump($out);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
	sscanf('-[in]-', '%[][-i-n]', $out);
    var_dump($out);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
string(7) "[hello]"
string(4) "-in-"
string(4) "-in-"
