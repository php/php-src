<?php
class IncludeHolder { public static $held = null; }
$fn = fn() { return 1; };
IncludeHolder::$held = $fn;
