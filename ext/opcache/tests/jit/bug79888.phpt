--TEST--
Bug #79888 (Incorrect execution with JIT enabled)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.jit=1205
--EXTENSIONS--
opcache
--FILE--
<?php
function testPrime(int $a): bool {
    if ($a < 2)  {
        return false;
    } else if ($a == 2) {
        return true;
    }
    for ($j = 2; $j < $a; $j++) {
        if (($a % $j) == 0) {
            return false;
        }
    }
    return true;
}

$max = 1000;
$cnt = 0;
echo "Testing Primes until: " . $max . "\n";
for ($i = 2; $i <= $max; $i++)
{
        if (testPrime($i)) $cnt++;
}
echo "Primect: {$cnt}\n";
?>
--EXPECT--
Testing Primes until: 1000
Primect: 168
