--TEST--
CURLOPT_OPENSOCKETFUNCTION: the handle/callback cycle is collectable
--EXTENSIONS--
curl
--FILE--
<?php
gc_collect_cycles();

/* The callback uses the handle it's set on: the cycle collector should still free both */
$ch = curl_init();
curl_setopt($ch, CURLOPT_OPENSOCKETFUNCTION, function () use ($ch): bool {
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
