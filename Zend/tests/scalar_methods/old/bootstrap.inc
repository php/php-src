<?php

$HANDLER_DIR = __DIR__ . '/../handlers';

function str($str) {
    echo "\nWorking on string \"$str\"\n\n";
    $GLOBALS['_str'] = $str;
}

function r($methodCode) {
    try {
        $result = eval("use extension string str\\Handler; return \$GLOBALS['_str']->$methodCode;");
        p($methodCode, $result);
    } catch (Exception $e) {
        echo $methodCode, ":\n", get_class($e), ': ', $e->getMessage(), "\n";
    }
}

function p($name, $result) {
    echo $name, ':', strlen($name) > 50 ? "\n" : ' ';
    var_dump($result);
}

function sep() {
    echo "\n";
}
