--TEST--
SPL: Allow valid extension of SplFileObject::fscanf
--FILE--
<?php

class A extends SplFileObject {
    public function fscanf($format) {

    }
}
?>
===DONE===
--EXPECT--
===DONE===
