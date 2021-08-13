--TEST--
Bug #71940 (Unserialize crushes on restore object reference)
--FILE--
<?php

class Identity
{
    private $role;

    public function __construct($role)
    {
        $this->role = $role;
    }
}

class Entry implements \Serializable
{
    private $identity;

    public function __construct(Identity $identity)
    {
        $this->identity = $identity;
    }

    public function serialize()
    {
        return serialize(array($this->identity));
    }

    public function unserialize($serialized)
    {
        list($this->identity) = unserialize($serialized);
    }
}

$identity = new Identity('test');
$identityRef = &$identity;

$entry1 = new Entry($identity);
$entry2 = new Entry($identityRef);

$serialized = serialize([$entry1, $entry2]);
print_r(unserialize($serialized));

?>
--EXPECTF--
Deprecated: %s implements the Serializable interface, which is deprecated. Implement __serialize() and __unserialize() instead (or in addition, if support for old PHP versions is necessary) in %s on line %d
Array
(
    [0] => Entry Object
        (
            [identity:Entry:private] => Identity Object
                (
                    [role:Identity:private] => test
                )

        )

    [1] => Entry Object
        (
            [identity:Entry:private] => Identity Object
                (
                    [role:Identity:private] => test
                )

        )

)
