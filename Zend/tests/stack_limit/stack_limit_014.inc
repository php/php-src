<?php

if (!class_exists("constructs_in_destructor")) {
    class constructs_in_destructor {
        public function __destruct() {
            $a = new constructs_in_destructor;
            $time = '';
            require(__FILE__);
        }
    }
}

$a = new constructs_in_destructor;
