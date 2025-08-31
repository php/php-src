--TEST--
SPL: Allow valid extension of SplFileObject::fscanf
--FILE--
<?php

class A extends SplFileObject {
    public function fscanf($format, &...$vars): array|int|null {

    }
}
?>
===DONE===
--EXPECT--
===DONE===
