--TEST--
Bug #64417 (BC break: ArrayAccess::&offsetGet() in a trait causes fatal error)
--FILE--
<?php
trait aa {
    private $container = array();
    public function offsetSet($offset, $value): void {
        if (is_null($offset)) {
            $this->container[] = $value;
        } else {
            $this->container[$offset] = $value;
        }
    }
    public function offsetExists($offset): bool {
        return isset($this->container[$offset]);
    }
    public function offsetUnset($offset): void {
        unset($this->container[$offset]);
    }
    public function &offsetGet($offset): mixed {
    $result = null;
        if (isset($this->container[$offset])) {
            $result = &$this->container[$offset];
        }
        return $result;
    }
}

class obj implements ArrayAccess {
    use aa;
}

$o = new obj;
$o['x'] = 1;
++$o['x'];
echo $o['x'], "\n";
?>
--EXPECT--
2
