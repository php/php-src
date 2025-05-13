<?php

interface DiableEarlyBinding {}

class C extends DateTime implements DiableEarlyBinding {
    public function getTimezone() {}
}
