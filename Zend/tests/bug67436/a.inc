<?php

class a {
    public function test($arg = c::TESTCONSTANT) {
        echo __METHOD__ . "($arg)\n";
    }

    static public function staticTest() {
    }
}
