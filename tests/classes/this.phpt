--TEST--
ZE2 $this cannot be exchanged
--FILE--
<?php

/* please don't shorten this test. It shows what would happen if
 * the fatal error would have been a warning.
 */
class Foo
{
    function replace($other)
    {
        echo __METHOD__ . "\n";
        $this = $other;
        print $this->prop;
        print $other->prop;
    }

    function indirect($other)
    {
        echo __METHOD__ . "\n";
        $this = $other;
        $result = $this = $other;
        print $result->prop;
        print $this->prop;
    }

    function retrieve(&$other)
    {
        echo __METHOD__ . "\n";
        $other = $this;
    }
}

$object = new Foo;
$object->prop = "Hello\n";

$other  = new Foo;
$other->prop = "World\n";

$object->replace($other);
$object->indirect($other);

print $object->prop; // still shows 'Hello'

$object->retrieve($other);
print $other->prop;  // shows 'Hello'

?>
===DONE===
--EXPECTF--
Fatal error: Cannot re-assign $this in %sthis.php on line %d
