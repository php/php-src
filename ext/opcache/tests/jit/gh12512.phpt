--TEST--
GH-12512: missing type store
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php
function bar(array &$a): ?bool {
    $ret = null;
    foreach ($a as $key => $val) {
        if ($val === 2) {
            unset($a[$key]);
        }
    }
    return $ret;
}

function foo($a, bool $b): bool {
    if ($b) return true;
    $n2 = count($a);
    do { 
        $n = $n2;
        $res = bar($a);
        $n2 = count($a);
    } while ($res === null && $n !== $n2);

    if ($res === null && $n === 0) {
        return false;
    }
    return true;
}

$a = [1,'a'=>5];
bar($a);
foo([1,'a'=>5], true);
foo([1,'a'=>5], false);
foo([2,'a'=>5], false);
?>
DONE
--EXPECT--
DONE
