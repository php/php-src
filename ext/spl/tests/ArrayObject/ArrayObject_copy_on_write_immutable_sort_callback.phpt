--TEST--
ArrayObject storage replaced by an immutable array while it is sorted through an iterator
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.protect_memory=1
opcache.file_update_protection=0
--FILE--
<?php
// The sort methods of an iterator do not guard the ArrayObject it comes from:
// the callback can replace its storage, here by the empty array and by a
// literal, which are immutable. The sorted array then replaces that storage.

function test(callable $replace) {
    $ao = new ArrayObject([3, 1, 2]);
    $it = $ao->getIterator();
    $replaced = false;
    $it->uasort(function ($a, $b) use ($ao, $replace, &$replaced) {
        if (!$replaced) {
            $replaced = true;
            $replace($ao);
        }

        return $a <=> $b;
    });
    $before = $ao->getArrayCopy();
    $ao[] = 'object';
    $after = $ao->getArrayCopy();
    $after[] = 'copy';
    echo json_encode($before), ' ', json_encode($ao->getArrayCopy()), ' ', json_encode($after), "\n";
}

test(fn ($ao) => $ao->exchangeArray([]));
test(fn ($ao) => $ao->exchangeArray([7, 8, 9]));
?>
--EXPECTF--

Deprecated: Method ArrayIterator::uasort() is deprecated since 8.6 in %s on line %d
{"1":1,"2":2,"0":3} {"1":1,"2":2,"0":3,"3":"object"} {"1":1,"2":2,"0":3,"3":"object","4":"copy"}

Deprecated: Method ArrayIterator::uasort() is deprecated since 8.6 in %s on line %d
{"1":1,"2":2,"0":3} {"1":1,"2":2,"0":3,"3":"object"} {"1":1,"2":2,"0":3,"3":"object","4":"copy"}
