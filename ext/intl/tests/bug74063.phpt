--TEST--
Bug #74063: NumberFormatter fails after retrieval from session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
session_start();

if(!isset($_SESSION["currencyFormatter"])) {
    echo "no formatter on session\n";
    $formatter = new NumberFormatter("en_GB", NumberFormatter::CURRENCY);
    $_SESSION["currencyFormatter"] = $formatter;
} else {
    die();
}

var_dump($formatter->format(1234567, NumberFormatter::CURRENCY));

session_write_close();
@session_start();

if(isset($_SESSION["currencyFormatter"])){
    $formatter = $_SESSION["currencyFormatter"];
    echo "loaded formatter from session\n";
} else {
    die();
}

var_dump($formatter->format(1234567, NumberFormatter::CURRENCY));
$_SESSION["currencyFormatter"] = new NumberFormatter("en_GB", NumberFormatter::PATTERN_DECIMAL, "#0.# kg");

session_write_close();
@session_start();

if(isset($_SESSION["currencyFormatter"])){
    $formatter = $_SESSION["currencyFormatter"];
    echo "loaded formatter from session\n";
} else {
    die();
}

var_dump($formatter->format(1234567));
$formatter->setPattern("#,##0.###");

session_write_close();
@session_start();

if(isset($_SESSION["currencyFormatter"])){
    $formatter = $_SESSION["currencyFormatter"];
    echo "loaded formatter from session\n";
} else {
    die();
}

var_dump($formatter->format(1234567));

session_write_close();
?>
--EXPECT--
no formatter on session
string(14) "£1,234,567.00"
loaded formatter from session
string(14) "£1,234,567.00"
loaded formatter from session
string(10) "1234567 kg"
loaded formatter from session
string(9) "1,234,567"

