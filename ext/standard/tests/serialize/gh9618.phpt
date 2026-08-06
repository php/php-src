--TEST--
GH-9618 (unserialize __wakeup bypass via malformed payload)
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

// Malformed payload: second property key length is 6 but the actual key is 4 bytes.
// The unserializer aborts partway through after constructing A and B.
// Before this fix, A::__destruct ran before B::__wakeup, letting the destructor
// reach B with an attacker-controlled $end (null). After the fix, B::__wakeup
// runs first so the guard value is in place when A::__destruct calls into B.
$payload = 'O:1:"A":2:{s:4:"info";O:1:"B":1:{s:3:"end";N;}s:6:"Aend";s:1:"1";}';

var_dump(@unserialize($payload));
?>
--EXPECTF--
B::__wakeup
B::__call end='wakeup-guard'
bool(false)
