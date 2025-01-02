<?php

// Need to cause a trace exit, so extend non existent class
class MyXSLTProcessor extends NonExistentClass {
    public function registerCycle() {
        [[$this]]; // Non trivial array
    }
}
