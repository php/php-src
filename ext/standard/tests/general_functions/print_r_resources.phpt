--TEST--
print_r(): Test printing resources
--INI--
precision=14
--FILE--
<?php

function check_print_r($variables) {
    $counter = 1;
    foreach($variables as $variable ) {
        echo "-- Iteration $counter --\n";
        ob_start();
        $should_be_true = print_r($variable, /* $return */ false);
        $output_content = ob_get_flush();
        echo PHP_EOL;
        var_dump($should_be_true);
        $print_r_with_return = print_r($variable, /* $return */ true);
        var_dump(/* $output_content === */ $print_r_with_return);
        $output_content = null;
        $counter++;
    }
}

/* Create a closed resource */
$dir_handle = opendir( __DIR__ );
closedir($dir_handle);

$resources = [
    STDERR,
    $dir_handle
];

check_print_r($resources);

?>
--EXPECTF--
-- Iteration 1 --

Warning: Resource to string conversion in %s on line %d
Resource id #3
bool(true)

Warning: Resource to string conversion in %s on line %d
string(14) "Resource id #3"
-- Iteration 2 --

Warning: Resource to string conversion in %s on line %d
Resource id #5
bool(true)

Warning: Resource to string conversion in %s on line %d
string(14) "Resource id #5"
