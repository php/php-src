<?php

class my {
    public function method() {
        return $this;
    }
}

function test2() {
    echo "Hello World 2\n";
}


$my = new my();
var_dump($my->method());

return true;
?>
