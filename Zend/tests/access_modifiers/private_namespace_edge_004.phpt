--TEST--
private(namespace) in global namespace allows access from global namespace only
--FILE--
<?php

class GlobalService {
    private(namespace) function test(): string {
        return "global";
    }
}

class GlobalConsumer {
    public function callTest(): string {
        $service = new GlobalService();
        return $service->test();
    }
}

$consumer = new GlobalConsumer();
echo $consumer->callTest() . "\n";

?>
--EXPECT--
global
