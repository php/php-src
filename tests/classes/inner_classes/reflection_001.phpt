--TEST--
reflection on inner classes
--FILE--
<?php

namespace n\s;

class Outer {
    class Middle {
        class Inner {}
    }
    private class PrivateMiddle {}
    protected class ProtectedMiddle {}
}

$outer = new \ReflectionClass(Outer::class);


function details($ref) {
    echo "Details for $ref\n";
    $ref = new \ReflectionClass($ref);
    var_dump($ref->getName());
    var_dump($ref->getShortName());
    var_dump($ref->isInnerClass());
    var_dump($ref->isPrivate());
    var_dump($ref->isProtected());
    var_dump($ref->isPublic());
}

details(Outer::class);
details('n\s\Outer\Middle\Inner');
details('n\s\Outer\PrivateMiddle');
details('n\s\Outer\ProtectedMiddle');

?>
--EXPECT--
Details for n\s\Outer
string(9) "n\s\Outer"
string(5) "Outer"
bool(false)
bool(false)
bool(false)
bool(true)
Details for n\s\Outer\Middle\Inner
string(22) "n\s\Outer\Middle\Inner"
string(5) "Inner"
bool(true)
bool(false)
bool(false)
bool(true)
Details for n\s\Outer\PrivateMiddle
string(23) "n\s\Outer\PrivateMiddle"
string(13) "PrivateMiddle"
bool(true)
bool(true)
bool(false)
bool(false)
Details for n\s\Outer\ProtectedMiddle
string(25) "n\s\Outer\ProtectedMiddle"
string(15) "ProtectedMiddle"
bool(true)
bool(false)
bool(true)
bool(false)
