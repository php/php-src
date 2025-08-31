<?php

class b extends a {
    private function __invoke() {}

    public function test($arg = c::TESTCONSTANT) {
        echo __METHOD__ . "()\n";
        parent::test();
    }
}
