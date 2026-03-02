<?php

if (!extension_loaded('mbstring')) {
    echo "Skipping mbstring dictionary generation\n";
    return;
}

$dict = "";
foreach (mb_list_encodings() as $encoding) {
    $dict .= "\"" . $encoding . ",\"\n";
}
file_put_contents(__DIR__ . "/dict/mbstring", $dict);
