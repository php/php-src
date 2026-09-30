--TEST--
Test Uri\QueryParams::append() - error - exception during Stringable local variable cleanup
--INI--
report_memleaks=1
--FILE--
<?php

class LocalValue {
    public function __destruct() {
        throw new Exception("Local destructor");
    }
}

class Value {
    public function __toString(): string {
        $local = new LocalValue();
        return str_repeat("x", 100);
    }
}

$params = new Uri\QueryParams();
try {
    $params = $params->append("key", new Value());
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

var_dump($params->count());

?>
--EXPECT--
Exception: Local destructor
int(0)
