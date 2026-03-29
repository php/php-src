--TEST--
Invokable in intersection types
--FILE--
<?php

interface Loggable {
    public function getLog(): string;
}

class LoggingHandler implements Loggable {
    public function __invoke(): string {
        return "result";
    }
    public function getLog(): string {
        return "log entry";
    }
}

function process(Invokable&Loggable $handler): void {
    echo $handler() . "\n";
    echo $handler->getLog() . "\n";
}

process(new LoggingHandler());

/* Object that is Invokable but not Loggable should be rejected */
class PlainHandler {
    public function __invoke(): string {
        return "plain";
    }
}

try {
    process(new PlainHandler());
} catch (TypeError $e) {
    echo "TypeError caught\n";
}

?>
--EXPECT--
result
log entry
TypeError caught
