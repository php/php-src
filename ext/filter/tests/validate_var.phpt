--TEST--
validate_var() and flags
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php
try {
    var_dump(validate_var("  234", FILTER_VALIDATE_INT));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(validate_var("234    ", FILTER_VALIDATE_INT));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(validate_var("  234  ", FILTER_VALIDATE_INT));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(validate_var("0xff", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_HEX)));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(validate_var("0Xff", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_HEX)));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(validate_var("0xFF", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_HEX)));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(validate_var("0XFF", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_HEX)));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(validate_var("07", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_OCTAL)));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(validate_var("0xff0000", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_HEX)));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(validate_var("0666", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_OCTAL)));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(validate_var("08", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_OCTAL)));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(validate_var("00", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_OCTAL)));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(validate_var("000", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_OCTAL)));
} catch (Exception $e) {
    var_dump($e->getMessage());
}

try {
    var_dump(validate_var("-0xff", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_HEX)));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(validate_var("-0Xff", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_HEX)));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(validate_var("-0xFF", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_HEX)));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(validate_var("-0XFF", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_HEX)));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(validate_var("-07", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_OCTAL)));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(validate_var("-0xff0000", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_HEX)));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(validate_var("-0666", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_OCTAL)));
} catch (Exception $e) {
    var_dump($e->getMessage());
}

try {
    var_dump(validate_var("6", FILTER_VALIDATE_INT, array("options" => array("min_range"=>1, "max_range"=>7))));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(validate_var("6", FILTER_VALIDATE_INT, array("options" => array("min_range"=>0, "max_range"=>5))));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(validate_var(-1, FILTER_VALIDATE_INT, array("options" => array("min_range"=>1, "max_range"=>7))));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(validate_var(-1, FILTER_VALIDATE_INT, array("options" => array("min_range"=>-4, "max_range"=>7))));
} catch (Exception $e) {
    var_dump($e->getMessage());
}

try {
    var_dump(validate_var("", FILTER_VALIDATE_INT, array("options" => array("min_range"=>-4, "max_range"=>7))));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(validate_var("", FILTER_VALIDATE_INT, array("options" => array("min_range"=>2, "max_range"=>7))));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(validate_var("", FILTER_VALIDATE_INT, array("options" => array("min_range"=>-5, "max_range"=>-3))));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(validate_var(345, FILTER_VALIDATE_INT, array("options" => array("min_range"=>500, "max_range"=>100))));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(validate_var("0ff", FILTER_VALIDATE_INT));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(validate_var("010", FILTER_VALIDATE_INT));
} catch (Exception $e) {
    var_dump($e->getMessage());
}


echo "Done\n";
?>
--EXPECT--
int(234)
int(234)
int(234)
int(255)
int(255)
int(255)
int(255)
int(7)
int(16711680)
int(438)
string(28) "Int validation: Out of range"
int(0)
int(0)
string(28) "Int validation: Out of range"
string(28) "Int validation: Out of range"
string(28) "Int validation: Out of range"
string(28) "Int validation: Out of range"
string(28) "Int validation: Out of range"
string(28) "Int validation: Out of range"
string(28) "Int validation: Out of range"
int(6)
string(28) "Int validation: Out of range"
string(28) "Int validation: Out of range"
int(-1)
string(27) "Int validation: Empty input"
string(27) "Int validation: Empty input"
string(27) "Int validation: Empty input"
string(28) "Int validation: Out of range"
string(28) "Int validation: Out of range"
string(28) "Int validation: Out of range"
Done
