--TEST--
WeakMap: releasing the values of a key stored in several WeakMaps must not expose the key
--FILE--
<?php

/* The key and $wm2 are GC roots when the key is freed. Releasing the key's
 * value in $wm1 runs the GC while the key is still a key of $wm2. */

class CollectOnDestruct {
    public function __destruct() {
        gc_collect_cycles();
    }
}

$wm1 = new WeakMap;
$wm2 = new WeakMap;
$key = new stdClass;
$wm1[$key] = new CollectOnDestruct;
$keep = new stdClass;
$wm2[$key] = $keep;
$tmp = $wm2; unset($tmp);
$tmp = $key; unset($tmp);
unset($key);
var_dump($keep, count($wm1), count($wm2));

/* Same, with the GC triggered by the root buffer filling up while the
 * value is released. */
$wm1 = new WeakMap;
$wm2 = new WeakMap;
$key = new stdClass;
$nodes = [];
for ($i = 0; $i < 20000; $i++) {
    $nodes[] = new stdClass;
}
$copy = [];
foreach ($nodes as $node) {
    $copy[] = $node;
}
$wm1[$key] = $copy;
unset($copy);
$keep = new stdClass;
$wm2[$key] = $keep;
$tmp = $wm2; unset($tmp);
$tmp = $key; unset($tmp);
unset($key);
var_dump($keep, count($wm1), count($wm2));

/* Same, without the GC: the value's destructor iterates $wm2 and must not
 * see the key being freed. */
class IterateOnDestruct {
    public function __destruct() {
        global $wm2, $leak;
        gc_disable();
        foreach ($wm2 as $k => $v) {
            $leak = $k;
        }
        var_dump(count($wm2));
    }
}

$wm1 = new WeakMap;
$wm2 = new WeakMap;
$key = new stdClass;
$wm1[$key] = new IterateOnDestruct;
$wm2[$key] = 1;
unset($key);
var_dump($leak ?? null);

?>
--EXPECTF--
object(stdClass)#%d (0) {
}
int(0)
int(0)
object(stdClass)#%d (0) {
}
int(0)
int(0)
int(0)
NULL
