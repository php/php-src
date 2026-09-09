--TEST--
GH-23639 (object_properties_load allows creating readonly reference properties)
--CREDITS--
arnaud-lb
ndossche
--FILE--
<?php

class Time_Duration {
    public int $seconds;
    public int $nanoseconds;
    public bool $negative;
}

$d = new Time_Duration();
$d->seconds = 1;
$a = [$d, &$d->seconds];

$payload = serialize($a);

try {
    unserialize(str_replace('Time_Duration', 'Time\\Duration', $payload));
} catch (Throwable $e) {
    do {
        echo $e::class, ": ", $e->getMessage(), "\n";
    } while ($e = $e->getPrevious());
}

?>
--EXPECT--
Exception: Invalid serialization data for Time\Duration object
Error: Cannot indirectly modify readonly property Time\Duration::$seconds
