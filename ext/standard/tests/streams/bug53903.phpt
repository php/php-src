--TEST--
Bug #53903 streamwrapper/stream_stat causes problems
--FILE--
<?php

class sw {

    public function stream_open($path, $mode, $options, &$opened_path) {
        return true;
    }

	public function stream_stat() {
		return array(
            'atime' => $this->undefined,
        );
	}

}
stream_wrapper_register('sx', 'sw') or die('failed');

fstat(fopen('sx://test', 'r'));

$s[] = 1; //  Cannot use a scalar value as an array

print_r($s);
--EXPECTF--
Notice: Undefined property: sw::$undefined in %s on line %d
Array
(
    [0] => 1
)

