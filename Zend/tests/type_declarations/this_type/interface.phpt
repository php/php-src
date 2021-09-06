--TEST--
$this type in interface
--FILE--
<?php

interface FluentInterface {
    public function doFluentThing(): $this;
}

class FluentImplementation implements FluentInterface {
    public function doFluentThing(): $this {
        echo "Done fluent thing\n";
        return $this;
    }
}

var_dump($fluent = new FluentImplementation);
var_dump($fluent->doFluentThing());

?>
--EXPECT--
object(FluentImplementation)#1 (0) {
}
Done fluent thing
object(FluentImplementation)#1 (0) {
}
