--TEST--
Clone method with parameter that accepts the original object.
--FILE--
<?php
class Cl {
    public $backupThis;

    public function __construct() {
        $this->backupThis = $this;
    }

    public function __clone(self $origThis) {
        var_dump($origThis === $this->backupThis);
        var_dump($origThis === $this);
    }
}

$obj = new Cl();
clone $obj;
echo "Done\n";
?>
--EXPECT--
bool(true)
bool(false)
Done
