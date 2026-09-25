--TEST--
.phpc: declare(strict_types=1) works as first statement in a .phpc file
--FILE--
<?php
$file = __DIR__ . '/' . basename(__FILE__, '.php') . '.phpc';
file_put_contents($file, "declare(strict_types=1);\nfunction add(int \$a, int \$b): int { return \$a + \$b; }\necho add(1, 2), \"\\n\";");
register_shutdown_function(fn() => @unlink($file));
require $file;
?>
--EXPECT--
3
