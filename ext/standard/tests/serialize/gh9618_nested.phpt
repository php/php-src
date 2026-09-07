--TEST--
GH-9618 (a failing nested unserialize() must not drain the outer call's queue)
--FILE--
<?php
class A
{
    public function __wakeup()
    {
        echo "A::__wakeup\n";
    }
}

class B implements Serializable
{
    public function serialize(): string
    {
        return 'x';
    }

    public function unserialize($data): void
    {
        @unserialize('O:1:"X":{BAD');
        echo "B::unserialize done\n";
    }

    public function __serialize(): array
    {
        return [];
    }

    public function __unserialize(array $data): void
    {
    }
}

$payload = 'a:2:{i:0;O:1:"A":0:{}i:1;C:1:"B":1:{x}}';

var_dump(@unserialize($payload) !== false);
?>
--EXPECT--
B::unserialize done
A::__wakeup
bool(true)
