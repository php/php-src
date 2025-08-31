--TEST--
Bug #65481 (shutdown segfault due to serialize)
--FILE--
<?php

class A {
    public $e = array();
}

class Token implements \Serializable {
    public function serialize()
    {
        $c = new A;

        for ($i = 0; $i < 4; $i++)
        {
            $e = new A;
            $c->e[] = $e;
            $e->e = $c->e;
        }

        return serialize(array(serialize($c)));
    }

    public function unserialize($str)
    {
        $r = unserialize($str);
        $r = unserialize($r[0]);
    }
}

$token = new Token;
$token = serialize($token);

?>
Done
--EXPECTF--
Deprecated: %s implements the Serializable interface, which is deprecated. Implement __serialize() and __unserialize() instead (or in addition, if support for old PHP versions is necessary) in %s on line %d
Done
