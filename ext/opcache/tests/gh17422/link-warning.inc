<?php

interface DisableEarlyBinding {}

class C extends DateTime implements DisableEarlyBinding {
    public function getTimezone() {}
}
