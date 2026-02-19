--TEST--
Promoted readonly property reassignment in constructor - conditional initialization
--FILE--
<?php

class Config {
    public function __construct(
        public readonly ?string $cacheDir = null,
    ) {
        $this->cacheDir ??= '/tmp/app_cache';
    }
}

$config1 = new Config();
var_dump($config1->cacheDir);

$config2 = new Config('/custom/cache');
var_dump($config2->cacheDir);

?>
--EXPECT--
string(14) "/tmp/app_cache"
string(13) "/custom/cache"
