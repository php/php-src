<?php

register_shutdown_function(function() {
    die(111);
});

die(222);
