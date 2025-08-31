--TEST--
tidy_get_body() crash
--EXTENSIONS--
tidy
--FILE--
<?php

// bug report taken from http://news.php.net/php.notes/130628

$inputs = array(
    '<frameset > </frameset>',
    '<html><frameset> </frameset> </html',
);


foreach ($inputs as $input) {

    $t = tidy_parse_string($input);
    $t->cleanRepair();
    var_dump(tidy_get_body($t));
}

echo "Done\n";
?>
--EXPECT--
NULL
NULL
Done
