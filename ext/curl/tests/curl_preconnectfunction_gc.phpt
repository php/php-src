--TEST--
CURLOPT_PRECONNECTFUNCTION: the handle/callback cycle is collectable
--EXTENSIONS--
curl
--FILE--
<?php
gc_collect_cycles();

/* The callback closes over the very handle it is registered on, so the handle is
 * only reclaimable if curl_get_gc() reports the callback to the cycle collector. */
$ch = curl_init();
curl_setopt($ch, CURLOPT_PRECONNECTFUNCTION, function () use ($ch): bool {
    return true;
});
$weak = WeakReference::create($ch);
unset($ch);

var_dump($weak->get() !== null);
gc_collect_cycles();
var_dump($weak->get() === null);

echo "Done";
?>
--EXPECT--
bool(true)
bool(true)
Done
