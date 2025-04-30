<?php

/**
 * @generate-class-entries
 * @undocumentable
 */

function dl_test_test1(): void {}

function dl_test_test2(string $str = ""): string {}

class DlTest {
    public function test(string $str = ""): string {}
}

class DlTestSuperClass {
    public int $a;
    public function test(string $str = ""): string {}
}

class DlTestSubClass extends DlTestSuperClass {
}

/** @alias DlTestClassAlias */
class DlTestAliasedClass {
}
