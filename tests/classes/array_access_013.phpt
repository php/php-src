--TEST--
ZE2 ArrayAccess and exceptions
--FILE--
<?php

class Test implements ArrayAccess
{
        public function offsetExists($offset): bool      { throw new Exception(__METHOD__); return false; }
        public function offsetGet($offset): mixed         { throw new Exception(__METHOD__); return $offset; }
        public function offsetSet($offset, $data ): void { throw new Exception(__METHOD__); }
        public function offsetUnset($offset): void       { throw new Exception(__METHOD__); }
}

$t = new Test;

try
{
    echo isset($t[0]);
}
catch(Exception $e)
{
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try
{
    echo $t[0];
}
catch(Exception $e)
{
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try
{
    $t[0] = 1;
}
catch(Exception $e)
{
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try
{
    unset($t[0]);
}
catch(Exception $e)
{
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Exception: Test::offsetExists
Exception: Test::offsetGet
Exception: Test::offsetSet
Exception: Test::offsetUnset
