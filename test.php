<?php
class phpdbg {
    public function isGreat($greeting = null) {
        printf(
            "%s: %s\n", __METHOD__, $greeting);
        return $this;
    }
}

$dbg = new phpdbg();

$test = 1;

var_dump(
    $dbg->isGreat("PHP Rocks !!"));
?>
