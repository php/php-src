--TEST--
Invoke destructors after bailout
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php

final class StatCounter {
    private static self $instance;
    public static function get(): self {
        return self::$instance ??= new self;
    }

    private array $stats = [];

    private function __construct() {}

    public function __destruct() {
        echo "Sending stats: ".json_encode($this->stats)."\n";
        exit(0);
    }

    public function inc(string $key, int $by = 1): void {
        $this->stats[$key] ??= 0;
        $this->stats[$key] += $by;
    }
}

StatCounter::get()->inc('test');
StatCounter::get()->inc('test2');

set_time_limit(1);
while (1);

?>
--EXPECTF--

Fatal error: Maximum execution time of 1 second exceeded in %s on line %d
Sending stats: {"test":1,"test2":1}
