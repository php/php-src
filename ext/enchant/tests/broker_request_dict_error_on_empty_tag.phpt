--TEST--
enchant_broker_request_dict() must throw ValueError on empty tag
--EXTENSIONS--
enchant
--SKIPIF--
<?php
if (!is_object(enchant_broker_init())) {die("skip, resource dont load\n");}
?>
--FILE--
<?php
$broker = enchant_broker_init();

try {
    var_dump(enchant_broker_request_dict($broker, ''));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
enchant_broker_request_dict(): Argument #2 ($tag) cannot be empty
