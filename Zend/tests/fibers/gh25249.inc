<?php
echo error_reporting(), "\n";
echo $main;
(new Fiber(function () {
    echo error_reporting(), "\n";
    echo $fiber;
}))->start();
