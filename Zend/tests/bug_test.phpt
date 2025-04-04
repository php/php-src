--TEST--
Test extending \Throwable directly
--FILE--
<?php

class MyThrowable implements Throwable {
    public function getMessage(): string {
        return "MyThrowable message";
    }

    public function getCode() {
        return 0;
    }

    public function getFile(): string {
        return __FILE__;
    }

    public function getLine(): int {
        return __LINE__;
    }

    public function getTrace(): array {
        return [];
    }

    public function getPrevious(): ?Throwable {
        return null;
    }

    public function getTraceAsString(): string {
        return "";
    }
}

try {
    throw new MyThrowable();
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
MyThrowable message
