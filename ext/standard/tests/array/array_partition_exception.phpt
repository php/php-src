--TEST--
Test array_partition() function : exception thrown inside the callback
--FILE--
<?php

$result = 'untouched';
$calls = 0;

try {
    $result = array_partition(['a' => new stdClass, 'b' => new stdClass, 'c' => new stdClass], function ($v, $k) use (&$calls) {
        $calls++;
        if ($k === 'b') {
            throw new RuntimeException("failed on $k");
        }
        return true;
    });
} catch (RuntimeException $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

var_dump($calls);
var_dump($result);

?>
--EXPECT--
RuntimeException: failed on b
int(2)
string(9) "untouched"
