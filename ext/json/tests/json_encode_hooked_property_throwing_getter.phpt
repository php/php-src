--TEST--
json_encode() releases the hooked property value when the get hook throws
--FILE--
<?php

class Value
{
    public function __destruct()
    {
        echo "Value::__destruct\n";
    }
}

class ThrowOnFree
{
    public function __destruct()
    {
        throw new Exception('thrown while freeing the get hook frame');
    }
}

class Container
{
    public $hooked {
        get {
            $local = new ThrowOnFree();
            return new Value();
        }
    }
}

try {
    json_encode(new Container());
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "done\n";

?>
--EXPECT--
Value::__destruct
Exception: thrown while freeing the get hook frame
done
