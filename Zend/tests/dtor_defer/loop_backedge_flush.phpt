--TEST--
Deferred destructors flush at loop back-edges
--INI--
opcache.enable_cli=0
--FILE--
<?php
$dtors = 0;

class C {
    public function __destruct() {
        global $dtors;
        $dtors++;
    }
}

for ($i = 0; $i < 3; $i++) {
    $object = new C;
    unset($object);
    echo $i, ":", $dtors, "\n";
}

echo "done:", $dtors, "\n";
?>
--EXPECT--
0:0
1:1
2:2
done:2
