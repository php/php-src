--TEST--
Bug #70436: Use After Free Vulnerability in unserialize()
--FILE--
<?php

class obj implements Serializable
{
    var $data;

    function serialize()
    {
        return serialize($this->data);
    }

    function unserialize($data)
    {
        $this->data = unserialize($data);
    }
}

$fakezval = ptr2str(1122334455);
$fakezval .= ptr2str(0);
$fakezval .= "\x00\x00\x00\x00";
$fakezval .= "\x01";
$fakezval .= "\x00";
$fakezval .= "\x00\x00";

$inner = 'C:3:"obj":3:{rya}';
$exploit = 'a:4:{i:0;i:1;i:1;C:3:"obj":'.strlen($inner).':{'.$inner.'}i:2;s:'.strlen($fakezval).':"'.$fakezval.'";i:3;R:5;}';

$data = unserialize($exploit);

var_dump($data);

function ptr2str($ptr)
{
    $out = '';

    for ($i = 0; $i < 8; $i++) {
        $out .= chr($ptr & 0xff);
        $ptr >>= 8;
    }

    return $out;
}
?>
DONE
--EXPECTF--
Deprecated: %s implements the Serializable interface, which is deprecated. Implement __serialize() and __unserialize() instead (or in addition, if support for old PHP versions is necessary) in %s on line %d

Warning: unserialize(): Error at offset 0 of 3 bytes in %s on line %d

Warning: unserialize(): Error at offset 93 of 94 bytes in %s on line %d
bool(false)
DONE
