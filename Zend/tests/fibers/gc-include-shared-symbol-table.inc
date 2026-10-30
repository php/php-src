<?php
$local = new stdClass;
$local->name = "local kept";
Fiber::suspend();
echo $local->name, "\n";
