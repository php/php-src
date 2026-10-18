--TEST--
OOM unserialize() Serializable
--INI--
memory_limit=200M
error_reporting=E_ALL&~E_DEPRECATED
--FILE--
<?php

class C implements Serializable
{
    public $data;
    public function serialize(): ?string {
        return 'hello';
    }
    public function unserialize(string $data): void {
        str_repeat('a', 200<<20);
    }
}

$payload = 'C:1:"C":5:{hello}';

var_dump(gettype(unserialize($payload)));

?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line 10
