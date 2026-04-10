--TEST--
GH-9618 (__wakeup runs before sibling destructors when the failure is nested)
--FILE--
<?php
class A
{
    public $info;

    public function __destruct()
    {
        if (is_object($this->info)) {
            $this->info->probe();
        }
    }
}

class B
{
    public $end;

    public function __wakeup()
    {
        $this->end = 'wakeup-guard';
        echo "B::__wakeup\n";
    }

    public function __call($method, $args)
    {
        echo "B::__call end=" . var_export($this->end, true) . "\n";
    }
}

class W implements Serializable
{
    public function serialize(): string
    {
        return 'x';
    }

    public function unserialize($data): void
    {
        @unserialize('O:1:"A":2:{s:4:"info";O:1:"B":1:{s:3:"end";N;}s:6:"Aend";s:1:"1";}');
        echo "W::unserialize done\n";
    }

    public function __serialize(): array
    {
        return [];
    }

    public function __unserialize(array $data): void
    {
    }
}

var_dump(@unserialize('C:1:"W":1:{x}') !== false);
?>
--EXPECT--
B::__wakeup
W::unserialize done
B::__call end='wakeup-guard'
bool(true)
