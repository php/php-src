--TEST--
ZE2 ArrayAccess and exceptions
--FILE--
<?php

class Test implements ArrayAccess
{
        public function offsetExists($offset)      { throw new Exception(__METHOD__); return false; }
        public function offsetGet($offset)         { throw new Exception(__METHOD__); return $offset; }
        public function offsetSet($offset, $data ) { throw new Exception(__METHOD__); }
        public function offsetUnset($offset)       { throw new Exception(__METHOD__); }
}

$t = new Test;

try
{
    echo isset($t[0]);
}
catch(Exception $e)
{
    echo "Caught in " . $e->getMessage() . "()\n";
}

try
{
    echo $t[0];
}
catch(Exception $e)
{
    echo "Caught in " . $e->getMessage() . "()\n";
}

try
{
    $t[0] = 1;
}
catch(Exception $e)
{
    echo "Caught in " . $e->getMessage() . "()\n";
}

try
{
    unset($t[0]);
}
catch(Exception $e)
{
    echo "Caught in " . $e->getMessage() . "()\n";
}
?>
--EXPECT--
Caught in Test::offsetExists()
Caught in Test::offsetGet()
Caught in Test::offsetSet()
Caught in Test::offsetUnset()
