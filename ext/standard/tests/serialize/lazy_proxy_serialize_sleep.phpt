--TEST--
serialize() of an initialized lazy proxy keeps the real instance alive across __sleep()
--FILE--
<?php

class C {
    public string $a = 'default-a';
    public string $b = 'default-b';

    public function __sleep(): array {
        return ['missing', 'a', 'b'];
    }
}

$rc = new ReflectionClass(C::class);
$proxy = $rc->newLazyProxy(function () {
    $real = new C();
    $real->a = 'old-a';
    $real->b = 'old-b';
    return $real;
});

$proxy->a;

$did = false;
$hold = [];
set_error_handler(function () use (&$did, &$hold, $rc, $proxy) {
    if ($did) {
        return true;
    }
    $did = true;
    $rc->resetAsLazyProxy($proxy, function () {
        return new C();
    });
    for ($i = 0; $i < 32; $i++) {
        $reused = new C();
        $reused->a = 'new-a';
        $reused->b = 'new-b';
        $hold[] = $reused;
    }
    return true;
});

echo serialize($proxy), "\n";

?>
--EXPECT--
O:1:"C":2:{s:1:"a";s:5:"old-a";s:1:"b";s:5:"old-b";}
