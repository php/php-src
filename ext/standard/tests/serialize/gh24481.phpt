--TEST--
GH-24481 (use-after-free in var_destroy() during unserialize() re-entrancy)
--FILE--
<?php

class Evil
{
    public function __destruct()
    {
        /* Re-entrant unserialize() while var_destroy() is running the deferred
         * destructor pass. The outer var_entries chunk has already been freed at
         * that point, and the nested call would reuse it. */
        $inner = 'a:1020:{';
        for ($i = 0; $i < 1020; $i++) {
            $inner .= "i:$i;i:$i;";
        }
        var_dump(count(@unserialize($inner . '}')));
    }
}

$count = 1015;
$payload = 'a:' . ($count + 3) . ':{';
for ($i = 0; $i < $count; $i++) {
    $payload .= "i:$i;i:$i;";
}
/* Duplicate key: the object is replaced, so PHP defers its destruction to the
 * var_destroy() destructor pass instead of destroying it inline. */
$payload .= 'i:88888;i:7;i:99999;O:4:"Evil":0:{}i:99999;i:42;}';

$outer = @unserialize($payload);
var_dump(is_array($outer) ? count($outer) : $outer);
?>
--EXPECT--
int(1020)
int(1017)
