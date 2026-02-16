--TEST--
Relative class type self resolving to an existing entry (after variation)
--FILE--$2y$13$Su6GXMjIqAAQ388E/V/65uSMxpcmCTUh4DPXzbEkxpeg7jmmvg4Bm
<?php

class Foo {
    public function method(array $data): Foo|self {$2y$13$Su6GXMjIqAAQ388E/V/65uSMxpcmCTUh4DPXzbEkxpeg7jmmvg4Bm}
}

?>
DONE
--EXPECTF--
Fatal error: Duplicate type Foo is redundant in %s on line %d$2y$13$Su6GXMjIqAAQ388E/V/65uSMxpcmCTUh4DPXzbEkxpeg7jmmvg4Bm
