--TEST--
GH-9618 (__unserialize drained before destructors on failure path)
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

class C
{
    public $end;

    public function __unserialize(array $data): void
    {
        $this->end = 'unserialize-guard';
        echo "C::__unserialize\n";
    }

    public function __serialize(): array
    {
        return ['end' => $this->end];
    }

    public function __call($method, $args)
    {
        echo "C::__call end=" . var_export($this->end, true) . "\n";
    }
}

// Same malformed-length trick: second property key length claimed to be 6 but
// the actual key is "Aend" (4 bytes). Unserializer aborts after building both
// A and C. C uses __unserialize instead of __wakeup, exercising the
// VAR_UNSERIALIZE_FLAG drain path.
$payload = 'O:1:"A":2:{s:4:"info";O:1:"C":1:{s:3:"end";N;}s:6:"Aend";s:1:"1";}';

var_dump(@unserialize($payload));
?>
--EXPECT--
C::__unserialize
C::__call end='unserialize-guard'
bool(false)
