--TEST--
OOM: Hard limit exceeded during compilation
--INI--
memory_limit=10M
--FILE--
<?php

if (!function_exists('str_increment')) {
    function str_increment(string $s) {
        $s++;
        return $s;
    }
}

$fname = __DIR__ . '/OOM_hard_limit_exceeded_during_complilation.test.php';
$fd = fopen($fname, 'w');

fprintf($fd, "<?php\n");

$name = str_repeat('A', 100);
for ($i = 0; $i < 10000; $i++) {
    fprintf(
        $fd,
        "class %s { public \$a; public \$b; function f() {} function g() {} }\n",
        $name,
    );
    $name = str_increment($name);
}

fclose($fd);

require $fname;

?>
--CLEAN--
<?php
unlink(__DIR__ . '/OOM_hard_limit_exceeded_during_complilation.test.php');
?>
--EXPECTF--
Hard memory limit of 20971520 bytes %s (tried to allocate %d bytes) in %s on line %d

Termsig=9
