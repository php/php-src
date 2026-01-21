--TEST--
Promoted readonly property reassignment in constructor - conditional initialization
--FILE--
<?php

class Config {
    public function __construct(
        public readonly ?string $cacheDir = null,
    ) {
        $this->cacheDir ??= sys_get_temp_dir() . '/app_cache';
    }
}

$config1 = new Config();
var_dump(str_contains($config1->cacheDir, 'app_cache'));

$config2 = new Config('/custom/cache');
var_dump($config2->cacheDir);

?>
--EXPECT--
bool(true)
string(13) "/custom/cache"
