<?php

class mysqli_warning
{
    public function __construct(object $mysqli_link);

    /** @return bool */
    public function next();
}
