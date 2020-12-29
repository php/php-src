<?php

if (!extension_loaded("simdjson")) {
    die("simdjson must be loaded");
}

$result = [
    ["filename", "json_decode()", "JsonParser::parse()", "JsonParser::isValid()"],
    ["--------------------", "---------------------", "---------------------", "---------------------"],
];

foreach (glob(__DIR__.'/../jsonexamples/*.json') as $item) {
    $jsonString = file_get_contents($item);

    $t1 = hrtime(true);
    json_decode($jsonString, true);
    $t2 = hrtime(true);
    $tDecode = round(($t2 - $t1) / 1000000, 3) . " ms";

    $t1 = hrtime(true);
    JsonParser::parse($jsonString, true);
    $t2 = hrtime(true);
    $tParse = round(($t2 - $t1) / 1000000, 3) . " ms";

    $t1 = hrtime(true);
    JsonParser::isValid($jsonString);
    $t2 = hrtime(true);
    $tIsValid = round(($t2 - $t1) / 1000000, 3) . " ms";

    $result[] = [basename($item), $tDecode, $tParse, $tIsValid];
}

foreach ($result as $line) {
    foreach ($line as $item) {
        echo str_pad($item, 21) . "|";
    }
    echo "\n";
}
