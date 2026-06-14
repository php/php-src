<?php

interface DisableEarlyBinding {}

class C extends DateTime implements DisableEarlyBinding {
    public function getTimezone() {}
    public function getTimestamp(C $arg): int {}
}
