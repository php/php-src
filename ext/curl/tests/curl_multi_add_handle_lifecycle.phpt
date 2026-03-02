--TEST--
curl_multi_add_handle does not hold onto the handle on failure
--EXTENSIONS--
curl
--FILE--
<?php

class MyClass {
    public function __destruct() {
        echo __METHOD__, PHP_EOL;
    }
}

$urls = [
    "file://".__DIR__."/curl_testdata1.txt",
    "file://".__DIR__."/curl_testdata2.txt",
];

$mh = curl_multi_init();

$toRemove = [];
foreach ($urls as $url) {
    $ch = curl_init($url);
    curl_setopt($ch, CURLOPT_PRIVATE, new MyClass());

    if (curl_multi_add_handle($mh, $ch) == CURLM_OK) {
        $toRemove[] = $ch;
    }
    if (curl_multi_add_handle($mh, $ch) == CURLM_OK) {
        $toRemove[] = $ch;
    }

    unset($ch);
}

echo "Removing", PHP_EOL;
foreach ($toRemove as $i => $ch) {
    curl_multi_remove_handle($mh, $ch);
    unset($ch);
    unset($toRemove[$i]);
}
echo "Removed", PHP_EOL;

?>
--EXPECTF--
Removing
MyClass::__destruct
MyClass::__destruct
Removed
