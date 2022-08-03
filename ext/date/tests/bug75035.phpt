--TEST--
Bug #75035 (Datetime fails to unserialize "extreme" dates)
--INI--
date.timezone=UTC
--FILE--
<?php
var_dump('PHP version', PHP_VERSION);

foreach ([PHP_INT_MIN, PHP_INT_MAX] as $extreme) {
    $i = 64;
    while ($i-- > 1) {
        $d = new DateTime('@' . ($extreme >> $i));
        $s = serialize($d);
        try {
            $u = unserialize($s);
        } catch (Error $e) {
            $u = "failed unserialization: " . $e->getMessage() . ' : ' . $s;
        }
        $original = $d->format('Y-m-d H:i:s');
        $serializedUnserialized = is_string($u) ? $u : $u->format('Y-m-d H:i:s');
        if ($original !== $serializedUnserialized) {
            var_dump('[' . ($extreme >> $i) . '] ' . $original . ' => ' . $serializedUnserialized);
        }
    }
}
?>
--EXPECTF--
string(11) "PHP version"
string(%d) "%s"
